//////////////////////////////////////////////////////////////////////////
//    File        	: RenderPassParticle.h
//    Created By    : Jack Spink
//    Created On 	: [27/01/2020]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassBase.h"

//////////////////////////////////////////////////////////////////////////

class RenderPassParticle : public RenderPassBase
{
    DECLARE_DERIVED(RenderPassParticle, RenderPassBase)
public:
    static inline const HashedString Id = HashedString::Create("RenderPass:Particle");

public:
    RenderPassParticle();

public:
    virtual s32 GetPriority() const { return 150; }
    virtual RenderPassType::Enum GetType() const { return RenderPassType::Particle; }
    virtual Ref<FramebufferAttachment> GetAttachment(GBuffer::Color attachment) override;

public:
    virtual void OnRenderCreate() override;

private:
    virtual void OnBegin() override;
    virtual void OnPerform() override;
    virtual void OnFinish() override;

public:
    void AddRenderFunction(RenderFunction* function);
    void RemoveRenderFunction(RenderFunction* function);

private:
    Ref<Framebuffer> framebuffer;
    std::vector<RenderFunction*> renderFunctions;
};

//////////////////////////////////////////////////////////////////////////
