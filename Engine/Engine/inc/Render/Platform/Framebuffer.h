#//////////////////////////////////////////////////////////////////////////
//    File        	: Framebuffer.h
//    Created By    : Jack Spink
//    Created On 	: [5/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include <array>
#include <functional>

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"

//////////////////////////////////////////////////////////////////////////

class Texture2D;

//////////////////////////////////////////////////////////////////////////

namespace GBuffer
{
    enum Enum
    {
        ColorBuffer,
        RenderBuffer,
    };

    enum Color
    {
        CB_Albedo,
        CB_Position,
        CB_Normal,
        CB_DepthStencil,

        // three extra color buffers for extra special rendering
        CB_ExtraColor0,
        CB_ExtraColor1,
        CB_ExtraColor2,

        MaxColorBuffer,
    };
}

//////////////////////////////////////////////////////////////////////////

struct ColorBuffer
{
    bool enabled = false;
    bool useFloatingPointFormat = false;
};

//////////////////////////////////////////////////////////////////////////

class FramebufferData
{
public:
    uint2 resolution;
    std::array<ColorBuffer, GBuffer::MaxColorBuffer> colorBuffers;
};

//////////////////////////////////////////////////////////////////////////

class FramebufferAttachment
{
public:
    virtual ~FramebufferAttachment() = default;

    virtual void Bind(const uint activeTexture) = 0;
    virtual void Unbind() = 0;
	virtual bool UseFloatingPointFormat() const = 0;
	virtual bool IsDepthAttachment() const = 0;

    virtual Ref<Texture2D> ToTexture() = 0;

    virtual void CopyAttachmentData(Ref<FramebufferAttachment> other) = 0;
};

//////////////////////////////////////////////////////////////////////////

class Framebuffer
{
public:
    virtual ~Framebuffer() = default;

    virtual void Bind() = 0;
    virtual void Unbind() = 0;

	virtual void Resize(const uint2& resolution) = 0;
    virtual Ref<FramebufferAttachment> GetColorBuffer(const GBuffer::Color buffer) = 0;

    virtual void ForEachColorBuffer(std::function<void(Ref<FramebufferAttachment>)> predicate) = 0;
    virtual void ForEachColorBuffer(std::function<void(const Ref<FramebufferAttachment>&)> predicate) const = 0;

};

//////////////////////////////////////////////////////////////////////////
