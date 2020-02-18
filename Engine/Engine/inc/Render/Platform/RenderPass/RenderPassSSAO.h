//////////////////////////////////////////////////////////////////////////
//    File        	: RenderPassSSAO.h
//    Created By    : Jack Spink
//    Created On 	: [27/01/2020]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassBase.h"

//////////////////////////////////////////////////////////////////////////

class RenderPassSSAO : public RenderPassBase
{
    DECLARE_DERIVED(RenderPassSSAO, RenderPassBase)
public:
    static inline const HashedString Id = HashedString::Create("RenderPass:SSAO");

public:
    RenderPassSSAO();

public:
    virtual s32 GetPriority() const { return 250; }
    virtual RenderPassType::Enum GetType() const { return RenderPassType::SSAO; }
    virtual Ref<FramebufferAttachment> GetAttachment(GBuffer::Color attachment) override;

public:
    virtual void OnRenderCreate() override;

private:
    virtual void OnBegin() override;
    virtual void OnPerform() override;
    virtual void OnFinish() override;

private:
    Ref<Framebuffer> ssaoFramebuffer;
    Ref<Framebuffer> blurFramebuffer;;
    Ref<Material> ssao, blur;
};

//////////////////////////////////////////////////////////////////////////
