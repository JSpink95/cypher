//////////////////////////////////////////////////////////////////////////
//    File        	: ParticleSystem.h
//    Created By    : Jack Spink
//    Created On 	: [22/10/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Component/TransformComponent.h"
#include "Render/Platform/RenderPass.h"

//////////////////////////////////////////////////////////////////////////

#include <vector>

//////////////////////////////////////////////////////////////////////////

class Material;
class VertexArray;
class VertexBuffer;
class ParticleEmitter;
class ParticleEmissionStage;
class ParticleUpdateStage;
class ParticleEvent;
class TransformComponent;

//////////////////////////////////////////////////////////////////////////

struct ParticleVertex
{
    float3 position = float3(0.0f);
    float2 size = float2(0.0f);
    float2 uv_scale = float2(1.0f);
    float2 uv_offset = float2(0.0f);
};

//////////////////////////////////////////////////////////////////////////

struct Particle
{
    ParticleVertex data;
    float3 velocity;
    f32 age;
};

//////////////////////////////////////////////////////////////////////////

class ParticleSystemComponent: public TransformComponent
{
    DECLARE_DERIVED_COMPONENT(ParticleSystemComponent, TransformComponent)
public:
    // {7C02AA16-7C81-4CF6-81DA-B8E8B1B40B92}
    static inline const GUID ClassId = { 0x7c02aa16, 0x7c81, 0x4cf6, { 0x81, 0xda, 0xb8, 0xe8, 0xb1, 0xb4, 0xb, 0x92 } };

public:

    // 
    // overridden - Component
    // 

    virtual void OnConstruct() override;
    virtual void OnUpdate(const f32 dt) override;
    virtual void OnRender(RenderPassType::Enum pass, Ref<Material> materialOverride) override;

    // 
    // overridden - TransformComponent
    // 

    virtual fmat4 CalculateTransformMatrix() const override;

    // request a new particle to be added in the next update iteration
    void RequestNewParticles(s32 count = 1);

    Ref<ParticleEmissionStage> GetEmissionStage() { return emissionStage; }
    Ref<ParticleUpdateStage> GetUpdateStage() { return updateStage; }

public:

    template<typename TEmitter>
    Ref<TEmitter> GetEmitterAsType() const { return std::dynamic_pointer_cast<TEmitter>(emitter); }

private:
    void RenderTask_InitialiseParticleMesh();

public:
    bool localSpaceParticles = false;
    Ref<Material> particleMaterial;

private:
    std::vector<Particle> particles;

    // particle emission
    s32 particleCountRequest = 0;
    f32 timeSinceLastEmission = 0.0f;
    f32 emissionRate = 0.005f;

    // particle modifiers
    Ref<ParticleEmissionStage> emissionStage;
    Ref<ParticleUpdateStage> updateStage;

    // particle events
    Ref<ParticleEvent> onParticleDeath;

    // particle meshes
    Ref<VertexBuffer> particleBuffer;
    Ref<VertexArray> particleMesh;

    // helper data
    WeakRef<TransformComponent> ownerTransform;
};

//////////////////////////////////////////////////////////////////////////
