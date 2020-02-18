//////////////////////////////////////////////////////////////////////////
//    File        	: OpenGlFramebuffer.cpp
//    Created By    : Jack Spink
//    Created On 	: [5/9/2019]
//////////////////////////////////////////////////////////////////////////

#ifdef _OPENGL

//////////////////////////////////////////////////////////////////////////

#include "glfw.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Application.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/OpenGL/OpenGlFramebuffer.h"
#include "Render/OpenGL/OpenGlTexture2D.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Window.h"

//////////////////////////////////////////////////////////////////////////

#include <vector>
#include <numeric>

//////////////////////////////////////////////////////////////////////////

OpenGlFramebufferAttachment::OpenGlFramebufferAttachment(const GBuffer::Enum type, const u32 id, const bool floatingPointFormat, const bool depthAttachment)
    : bufferType(type)
    , bufferId(id)
	, useFloatingPointFormat(floatingPointFormat)
	, isDepthAttachment(depthAttachment)
{
    texture = std::make_shared<OpenGlTexture2D>(bufferId);
}

//////////////////////////////////////////////////////////////////////////

void OpenGlFramebufferAttachment::Bind(const uint activeTexture)
{
    glActiveTexture(GL_TEXTURE0 + activeTexture);
    switch (bufferType)
    {
        case GBuffer::ColorBuffer:
            glBindTexture(GL_TEXTURE_2D, bufferId);
            break;
    }
}

//////////////////////////////////////////////////////////////////////////

void OpenGlFramebufferAttachment::Unbind()
{
    switch (bufferType)
    {
        case GBuffer::ColorBuffer:
            glBindTexture(GL_TEXTURE_2D, 0);
            break;
    }
}

//////////////////////////////////////////////////////////////////////////

void OpenGlFramebufferAttachment::CopyAttachmentData(Ref<FramebufferAttachment> other)
{
    Ref<OpenGlFramebufferAttachment> otherGl = std::dynamic_pointer_cast<OpenGlFramebufferAttachment>(other);
    OpenGlFramebuffer* otherOwner = otherGl->owner;

    // #todo - this is quite hacky, but it works SPECIFICALLY for my particles so it's fine

    glBlitNamedFramebuffer(
        otherOwner->id, owner->id,
        0, 0, otherOwner->cachedResolution.x, otherOwner->cachedResolution.y,
        0, 0, owner->cachedResolution.x, owner->cachedResolution.y,
        GL_DEPTH_BUFFER_BIT, GL_NEAREST
    );
}

//////////////////////////////////////////////////////////////////////////

OpenGlFramebuffer::OpenGlFramebuffer(const FramebufferData& initData)
    : cachedResolution(initData.resolution)
{
    static auto CreateColorBuffer = [](const uint2& resolution, const GBuffer::Color buffer, const u32 bufferIndex, const bool useFloatingPointFormat) -> u32
    {
        u32 textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        if (buffer != GBuffer::CB_DepthStencil)
        {
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                useFloatingPointFormat ? GL_RGBA32F : GL_RGBA,
                resolution.x, resolution.y,
                0,
                GL_RGBA,
                useFloatingPointFormat ? GL_FLOAT : GL_UNSIGNED_BYTE,
                NULL
            );
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, resolution.x, resolution.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        }

        // attach it to currently bound framebuffer object
        if (buffer != GBuffer::CB_DepthStencil)
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + bufferIndex, GL_TEXTURE_2D, textureId, 0);
        else
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureId, 0);

        glBindTexture(GL_TEXTURE_2D, 0);
        return textureId;
    };

    glGenFramebuffers(1, &id);
    glBindFramebuffer(GL_FRAMEBUFFER, id);

    u32 colorBufferCount = 0;
    u32 currentColorBuffer = 0;
    for (const ColorBuffer& buffer : initData.colorBuffers)
    {
        if (buffer.enabled)
        {
            const bool floatingPointFormat = buffer.useFloatingPointFormat;
			const bool depthAttachment = (GBuffer::Color)currentColorBuffer == GBuffer::CB_DepthStencil;

            s32 bufferId = CreateColorBuffer(initData.resolution, (GBuffer::Color)currentColorBuffer, colorBufferCount, floatingPointFormat);
            colorBuffers.at(currentColorBuffer) = std::make_shared<OpenGlFramebufferAttachment>(GBuffer::ColorBuffer, bufferId, floatingPointFormat, depthAttachment);
            colorBuffers.at(currentColorBuffer)->owner = this;

            if (!depthAttachment)
                ++colorBufferCount;
        }

        ++currentColorBuffer;
    }

    std::vector<GLenum> drawBuffers(colorBufferCount);
    std::iota(drawBuffers.begin(), drawBuffers.end(), GL_COLOR_ATTACHMENT0);

    if (drawBuffers.size() > 0u)
    {
        glDrawBuffers(drawBuffers.size(), &drawBuffers.at(0));
    }
    else
    {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//////////////////////////////////////////////////////////////////////////

void OpenGlFramebuffer::Resize(const uint2& resolution)
{
}

//////////////////////////////////////////////////////////////////////////

void OpenGlFramebuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glViewport(0, 0, cachedResolution.x, cachedResolution.y);
}

//////////////////////////////////////////////////////////////////////////

void OpenGlFramebuffer::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    uint2 windowResolution = GetApplication()->GetWindowContext()->GetWindowSize();
    glViewport(0, 0, windowResolution.x, windowResolution.y);
}

//////////////////////////////////////////////////////////////////////////

Ref<FramebufferAttachment> OpenGlFramebuffer::GetColorBuffer(const GBuffer::Color buffer)
{
    return colorBuffers.at(buffer);
}

//////////////////////////////////////////////////////////////////////////

void OpenGlFramebuffer::ForEachColorBuffer(std::function<void(Ref<FramebufferAttachment>)> predicate)
{
    for (Ref<OpenGlFramebufferAttachment> attachment : colorBuffers)
        predicate(attachment);
}

//////////////////////////////////////////////////////////////////////////

void OpenGlFramebuffer::ForEachColorBuffer(std::function<void(const Ref<FramebufferAttachment>&)> predicate) const
{
    for (const Ref<OpenGlFramebufferAttachment>& attachment : colorBuffers)
        predicate(attachment);
}

//////////////////////////////////////////////////////////////////////////

#endif // _OPENGL

//////////////////////////////////////////////////////////////////////////
