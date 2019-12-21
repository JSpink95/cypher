//////////////////////////////////////////////////////////////////////////
//    File        	: OpenGlFramebuffer.h
//    Created By    : Jack Spink
//    Created On 	: [5/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Framebuffer.h"

//////////////////////////////////////////////////////////////////////////

class OpenGlFramebufferAttachment : public FramebufferAttachment
{
    friend class OpenGlFramebuffer;
public:
    OpenGlFramebufferAttachment(const GBuffer::Enum type, const u32 id, const bool useFloatingPointFormat, const bool isDepthAttachment);

    inline GBuffer::Enum GetBufferType() const { return bufferType; }
    inline u32 GetBufferId() const { return bufferId; }

    virtual void Bind(const uint activeTexture) override;
    virtual void Unbind() override;

	inline virtual bool UseFloatingPointFormat() const override { return isDepthAttachment; }
	inline virtual bool IsDepthAttachment() const override { return isDepthAttachment; }

    inline virtual Ref<Texture2D> ToTexture() override { return texture; }

    virtual void CopyAttachmentData(Ref<FramebufferAttachment> other) override;

private:
    OpenGlFramebuffer* owner;
    GBuffer::Enum bufferType;
    u32 bufferId;
    Ref<Texture2D> texture;
	bool useFloatingPointFormat;
	bool isDepthAttachment;
};

//////////////////////////////////////////////////////////////////////////

class OpenGlFramebuffer : public Framebuffer
{
    friend OpenGlFramebufferAttachment;
public:
    OpenGlFramebuffer(const FramebufferData& initData);

    virtual void Bind() override;
    virtual void Unbind() override;

	virtual void Resize(const uint2& resolution) override;
    virtual Ref<FramebufferAttachment> GetColorBuffer(const GBuffer::Color buffer) override;

    virtual void ForEachColorBuffer(std::function<void(Ref<FramebufferAttachment>)> predicate) override;
    virtual void ForEachColorBuffer(std::function<void(const Ref<FramebufferAttachment>&)> predicate) const override;

private:
    u32 id;
    uint2 cachedResolution;
    std::array<Ref<OpenGlFramebufferAttachment>, GBuffer::MaxColorBuffer> colorBuffers;
};

//////////////////////////////////////////////////////////////////////////
