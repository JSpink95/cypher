//////////////////////////////////////////////////////////////////////////
//    File        	: RenderPassShadow.h
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

struct ShadowLight
{
    vec3 position;
    f32 type;
    mat4 sourceMatrix;
};

//////////////////////////////////////////////////////////////////////////

struct ShadowBufferData
{
    ShadowLight shadowCasters[16];
    int shadowCasterCount;
};

//////////////////////////////////////////////////////////////////////////

class RenderPassShadow : public RenderPassBase
{
    DECLARE_DERIVED(RenderPassShadow, RenderPassBase)
public:
    static inline const HashedString Id = HashedString::Create("RenderPass:Shadow");

public:
    RenderPassShadow();

public:
    virtual s32 GetPriority() const { return 100; }
    virtual RenderPassType::Enum GetType() const { return RenderPassType::Shadow; }
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

public:
    void AddDirectionalLight(const std::string& id, const float3& direction, const f32 distance);
    void GetShadowBufferData(ShadowBufferData& output);

private:
    Ref<Framebuffer> framebuffer;
    std::vector<RenderFunction*> renderFunctions;
    std::unordered_map<std::string, float3> lights;
};

//////////////////////////////////////////////////////////////////////////
