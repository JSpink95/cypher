//////////////////////////////////////////////////////////////////////////
//    File        	: RenderPassUnlit.h
//    Created By    : Jack Spink
//    Created On 	: [27/01/2020]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassBase.h"

//////////////////////////////////////////////////////////////////////////

class Unlit
{
public:
    static const GBuffer::Color ColorOutput;
    static const GBuffer::Color DepthOutput;
};

//////////////////////////////////////////////////////////////////////////

class RenderPassUnlit : public RenderPassBase
{
    DECLARE_DERIVED(RenderPassUnlit, RenderPassBase)
public:
    static inline const HashedString Id = HashedString::Create("RenderPass:Unlit");

public:
    RenderPassUnlit();

public:
    virtual void OnRenderCreate() override;
    virtual s32 GetPriority() const { return 50; }
    virtual RenderPassType::Enum GetType() const { return RenderPassType::Unlit; }
    virtual Ref<FramebufferAttachment> GetAttachment(GBuffer::Color attachment) override;

public:
    virtual void OnBegin() override;
    virtual void OnPerform() override;
    virtual void OnFinish() override;

public:
    void AddRenderFunction(RenderFunction* function, bool transparent = false);
    void RemoveRenderFunction(RenderFunction* function, bool transparent = false);


private:
    Ref<Framebuffer> framebuffer;
    std::vector<RenderFunction*> opaqueRenderFunctions;
    std::vector<RenderFunction*> transparentRenderFunctions;
};

//////////////////////////////////////////////////////////////////////////
