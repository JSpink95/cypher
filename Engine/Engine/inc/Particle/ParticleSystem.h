//////////////////////////////////////////////////////////////////////////
//    File        	: ParticleSystem.h
//    Created By    : Jack Spink
//    Created On 	: [22/10/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Component/TransformComponent.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass.h"
#include "Render/Platform/Buffer.h"

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
    static inline VertexBufferLayout layout = {
        { "aPosition", ShaderData::Float3 },
        { "aSize", ShaderData::Float2 },
        { "aUvScale", ShaderData::Float2 },
        { "aUvOffset", ShaderData::Float2 },
    };

    float3 position = float3(0.0f);
    float2 size = float2(1.0f);
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

    inline void SetMaterial(Ref<Material> newMaterial)
    {
        material = newMaterial;
    }

    inline void SetLocalSpaceParticles(const bool isLocalSpace)
    {
        localSpaceParticles = isLocalSpace;
    }

    inline void SetEmissionRate(const f32 rate)
    {
        emissionRate = glm::max(rate, 0.0f);
    }

    inline void SetMaxParticlesAlive(const s32 alive)
    {
        maxAliveParticles = alive;
    }

    inline void SetParticleDeathEvent(Ref<ParticleEvent> newEvent)
    {
        onParticleDeath = newEvent;
    }

private:
    void RenderTask_InitialiseParticleMesh();

public:

private:
    std::vector<Particle> particles;

public:
    // render data
    Ref<Material> material;
    bool localSpaceParticles = false;

private:
    // particle emission
    s32 particleCountRequest = 0;
    u64 numParticlesSpawned = 0u;
    f32 timeSinceLastEmission = 0.0f;

public:
    // particle emission editables
    f32 emissionRate = 0.005f;
    s32 maxAliveParticles = 0;   // 0 means no cap

    // particle modifiers
    Ref<ParticleEmissionStage> emissionStage;
    Ref<ParticleUpdateStage> updateStage;

    // particle events
    Ref<ParticleEvent> onParticleDeath;

private:

    // particle meshes
    Ref<VertexBuffer> particleBuffer;
    Ref<VertexArray> particleMesh;

    // helper data
    WeakRef<TransformComponent> ownerTransform;
};

//////////////////////////////////////////////////////////////////////////
