//////////////////////////////////////////////////////////////////////////
//    File        	: RenderPassFinal.h
//    Created By    : Jack Spink
//    Created On 	: [28/01/2020]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassBase.h"

//////////////////////////////////////////////////////////////////////////

class RenderPassFinal : public RenderPassBase
{
    DECLARE_DERIVED(RenderPassFinal, RenderPassBase)
public:
    static inline const HashedString Id = HashedString::Create("RenderPass:Final");

public:
    RenderPassFinal();

public:
    virtual s32 GetPriority() const { return INT_MAX; }
    virtual RenderPassType::Enum GetType() const { return RenderPassType::Final; }

public:
    virtual void OnRenderCreate() override;

public:
    virtual void OnBegin() override;
    virtual void OnPerform() override;
    virtual void OnFinish() override;

private:
    Ref<Framebuffer> framebuffer;
    Ref<Material> overlay, output;
};

//////////////////////////////////////////////////////////////////////////
