//////////////////////////////////////////////////////////////////////////
//    File        	: RenderPassSSL.h
//    Created By    : Jack Spink
//    Created On 	: [28/01/2020]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassBase.h"

//////////////////////////////////////////////////////////////////////////

class SSL
{
public:
    static const GBuffer::Color ColorOutput;
};

//////////////////////////////////////////////////////////////////////////

class RenderPassSSL : public RenderPassBase
{
    DECLARE_DERIVED(RenderPassSSL, RenderPassBase)
public:
    static inline const HashedString Id = HashedString::Create("RenderPass:SSL");

public:
    RenderPassSSL();

public:
    virtual s32 GetPriority() const { return 200; }
    virtual RenderPassType::Enum GetType() const { return RenderPassType::SSL; }
    virtual Ref<FramebufferAttachment> GetAttachment(GBuffer::Color attachment) override;

public:
    virtual void OnRenderCreate() override;

private:
    virtual void OnBegin() override;
    virtual void OnPerform() override;
    virtual void OnFinish() override;

private:
    Ref<Framebuffer> framebuffer;
    Ref<Material> ssl;
};

//////////////////////////////////////////////////////////////////////////
