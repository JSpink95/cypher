//////////////////////////////////////////////////////////////////////////
//    File        	: RenderPassCoreCombiner.h
//    Created By    : Jack Spink
//    Created On 	: [19/02/2020]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassBase.h"

//////////////////////////////////////////////////////////////////////////

class RenderPassCoreCombiner: public RenderPassBase
{
    DECLARE_DERIVED(RenderPassCoreCombiner, RenderPassBase)
public:
    static inline const HashedString Id = HashedString::Create("RenderPass:CoreCombiner");

public:
    RenderPassCoreCombiner();

public:
    virtual s32 GetPriority() const { return 201; }
    virtual RenderPassType::Enum GetType() const { return RenderPassType::CoreCombiner; }
    virtual Ref<FramebufferAttachment> GetAttachment(GBuffer::Color attachment) override;

public:
    virtual void OnRenderCreate() override;

public:
    virtual void OnBegin() override;
    virtual void OnPerform() override;
    virtual void OnFinish() override;

private:
    Ref<Framebuffer> overlaySSAO;
    Ref<Framebuffer> overlayUnlit;
    Ref<Framebuffer> overlayParticle;
    Ref<Material> overlay;
};

//////////////////////////////////////////////////////////////////////////
