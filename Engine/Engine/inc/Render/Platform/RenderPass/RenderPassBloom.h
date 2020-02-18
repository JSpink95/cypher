//////////////////////////////////////////////////////////////////////////
//    File        	: RenderPassBloom.h
//    Created By    : Jack Spink
//    Created On 	: [17/02/2020]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassBase.h"

//////////////////////////////////////////////////////////////////////////

class Texture2D;

//////////////////////////////////////////////////////////////////////////

class Bloom
{
public:
    static const GBuffer::Color Output;
};

//////////////////////////////////////////////////////////////////////////

class RenderPassBloom : public RenderPassBase
{
    DECLARE_DERIVED(RenderPassBloom, RenderPassBase)
public:
    static inline const HashedString Id = HashedString::Create("RenderPass:Bloom");

public:
    RenderPassBloom();

public:
    virtual void OnRenderCreate() override;
    virtual s32 GetPriority() const { return 225; }
    virtual RenderPassType::Enum GetType() const { return RenderPassType::Bloom; }
    virtual Ref<FramebufferAttachment> GetAttachment(GBuffer::Color attachment) override;

public:
    virtual void OnBegin() override;
    virtual void OnPerform() override;
    virtual void OnFinish() override;

private:
    Ref<Material> guassianBlurMaterial;
    Ref<Material> bloomBlendMaterial;
    Ref<Framebuffer> framebuffers[2];
    Ref<Framebuffer> bloom;
};

//////////////////////////////////////////////////////////////////////////
