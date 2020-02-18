//////////////////////////////////////////////////////////////////////////
//    File        	: RenderPassLit.h
//    Created By    : Jack Spink
//    Created On 	: [27/01/2020]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassBase.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/TypeId.h"

//////////////////////////////////////////////////////////////////////////

class Object;
class Framebuffer;

//////////////////////////////////////////////////////////////////////////

class Lit
{
public:
    static const GBuffer::Color ColorOutput;
    static const GBuffer::Color PositionOutput;
    static const GBuffer::Color NormalOutput;
    static const GBuffer::Color DepthOutput;
};

//////////////////////////////////////////////////////////////////////////

class RenderPassLit : public RenderPassBase
{
    DECLARE_DERIVED(RenderPassLit, RenderPassBase)
public:
    static inline const HashedString Id = HashedString::Create("RenderPass:Lit");

public:
    RenderPassLit();

public:
    virtual void OnRenderCreate() override;
    virtual s32 GetPriority() const { return 0; }
    virtual RenderPassType::Enum GetType() const { return RenderPassType::Lit;  }
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
