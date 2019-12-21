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
class ParticleUpdater;
class TransformComponent;

//////////////////////////////////////////////////////////////////////////

struct ParticleVertex
{
    float3 position;
    float2 size;
};

//////////////////////////////////////////////////////////////////////////

struct Particle
{
    ParticleVertex data;
    float3 velocity;
    f32 age;
};

//////////////////////////////////////////////////////////////////////////

struct ParticleManager
{
    using iterator = std::vector<Particle>::iterator;
    using const_iterator = std::vector<Particle>::const_iterator;

    std::vector<Particle> particles;

    void Update(f32 dt);
    void BuildParticleBatch(std::vector<ParticleVertex>& vertices);

    inline iterator begin() { return particles.begin(); }
    inline const_iterator begin() const { return particles.begin(); }

    inline iterator end() { return particles.end(); }
    inline const_iterator end() const { return particles.end(); }
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

    void PushParticle(const float3& position, const float2& size, const float3& velocity, const f32 age);

    Ref<ParticleEmitter> GetEmitter() const { return emitter; }
    Ref<ParticleUpdater> GetUpdater() const { return updater; }

    void SetEmitter(Ref<ParticleEmitter> newEmitter) { emitter = newEmitter; }
    void SetUpdater(Ref<ParticleUpdater> newUpdater) { updater = newUpdater; }

public:

    template<typename TEmitter>
    Ref<TEmitter> GetEmitterAsType() const { return std::dynamic_pointer_cast<TEmitter>(emitter); }

    template<typename TUpdater>
    Ref<TUpdater> GetUpdatersAsType() const { return std::dynamic_pointer_cast<TUpdater>(updater); }

private:
    void RenderTask_InitialiseParticleMesh();

public:
    bool localSpaceParticles = false;
    Ref<Material> particleMaterial;

private:
    ParticleManager manager;
    Ref<ParticleEmitter> emitter;
    Ref<ParticleUpdater> updater;

    Ref<VertexBuffer> particleBuffer;
    Ref<VertexArray> particleMesh;

    WeakRef<TransformComponent> ownerTransform;
};

//////////////////////////////////////////////////////////////////////////
