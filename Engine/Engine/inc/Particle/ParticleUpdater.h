//////////////////////////////////////////////////////////////////////////
//    File        	: ParticleUpdater.h
//    Created By    : Jack Spink
//    Created On 	: [1/11/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"
#include "Core/ObjectMacros.h"

//////////////////////////////////////////////////////////////////////////

#include <random>
#include <vector>

//////////////////////////////////////////////////////////////////////////

struct Particle;

//////////////////////////////////////////////////////////////////////////

class ParticleEvent
{
public:
    virtual void OnEvent(Particle& particle) = 0;
};

//////////////////////////////////////////////////////////////////////////

class ParticleEmissionProcess
{
    DEFINE_CLASS_UID(ParticleEmissionProcess)
public:
    virtual void Initialise(Particle& particle) = 0;
};

//////////////////////////////////////////////////////////////////////////

class ParticleUpdateProcess
{
    DEFINE_CLASS_UID(ParticleUpdateProcess)
public:
    virtual void Update(const f32 dt, Particle& particle) = 0;
};

//////////////////////////////////////////////////////////////////////////

template<typename T>
class TParticleStage
{
public:
    using ProcessType = Ref<T>;

public:
    inline void AddOutput(ProcessType process)
    {
        processes.push_back(process);
    }

    inline void RemoveOutput(ProcessType process)
    {
        processes.erase(std::remove(processes.begin(), processes.end(), process), processes.end());
    }

public:

    template<typename O>
    inline Ref<O> PushOutput()
    {
        Ref<O> output = std::make_shared<O>();
        processes.push_back(output);

        return output;
    }

protected:
    std::vector<ProcessType> processes;
};

//////////////////////////////////////////////////////////////////////////

class ParticleEmissionStage : public TParticleStage<ParticleEmissionProcess>
{
public:
    void Initialise(Particle& particle);
};

//////////////////////////////////////////////////////////////////////////

class ParticleUpdateStage: public TParticleStage<ParticleUpdateProcess>
{
public:
    void Update(const f32 dt, Particle& particle);
};

//////////////////////////////////////////////////////////////////////////

class ParticlePointEmitter : public ParticleEmissionProcess
{
    DEFINE_CLASS_UID(ParticlePointEmitter)
public:
    virtual void Initialise(Particle& particle) override;

public:
    float3 point = float3(0.0f);
};

//////////////////////////////////////////////////////////////////////////

class ParticleSphereEmitter : public ParticleEmissionProcess
{
    DEFINE_CLASS_UID(ParticleSphereEmitter)
public:
    virtual void Initialise(Particle& particle) override;

public:
    float2 radius = float2(0.0f, 1.0f);
    float2 direction = float2(-1.0f, 1.0f);
    bool clampToEdge = false;
};

//////////////////////////////////////////////////////////////////////////

class ParticleSetLifetime : public ParticleEmissionProcess
{
    DEFINE_CLASS_UID(ParticleSetLifetime)
public:
    virtual void Initialise(Particle& particle) override;

public:
    f32 lifetime = 1.0f;
};

class ParticleSetLifetimeRandom : public ParticleEmissionProcess
{
    DEFINE_CLASS_UID(ParticleSetLifetimeRandom)
public:
    virtual void Initialise(Particle& particle) override;

public:
    f32 minLifetime = 0.5f;
    f32 maxLifetime = 1.0f;
};

//////////////////////////////////////////////////////////////////////////

class ParticleSetSize : public ParticleEmissionProcess
{
    DEFINE_CLASS_UID(ParticleSetSize)
public:
    virtual void Initialise(Particle& particle) override;

public:
    float2 size = float2(1.0f);
};

class ParticleSetSizeRandom : public ParticleEmissionProcess
{
    DEFINE_CLASS_UID(ParticleSetSizeRandom)
public:
    virtual void Initialise(Particle& particle) override;

public:
    f32 minSize = 0.0f;
    f32 maxSize = 1.0f;
};

//////////////////////////////////////////////////////////////////////////

class ParticleSetUV : public ParticleEmissionProcess
{
    DEFINE_CLASS_UID(ParticleSetUV)
public:
    virtual void Initialise(Particle& particle) override;

public:
    float2 scale = float2(1.0f);
    float2 offset = float2(0.0f);
};

//////////////////////////////////////////////////////////////////////////

class ParticleSetVelocity : public ParticleEmissionProcess
{
    DEFINE_CLASS_UID(ParticleSetVelocity)
public:
    virtual void Initialise(Particle& particle) override;

public:
    f32 strength = 1.0f;
    float3 direction = float3(0.0f, 1.0f, 0.0f);
};

//////////////////////////////////////////////////////////////////////////

class ParticleSetVelocityRandom : public ParticleEmissionProcess
{
    DEFINE_CLASS_UID(ParticleSetVelocityRandom)
public:
    virtual void Initialise(Particle& particle) override;

public:
    float2 strength = float2(0.0f, 1.0f);
    float3 minDirections = float3(-1.0f);
    float3 maxDirections = float3(1.0f);
};

//////////////////////////////////////////////////////////////////////////

class ParticleLinearDrag : public ParticleUpdateProcess
{
    DEFINE_CLASS_UID(ParticleLinearDrag)
public:
    virtual void Update(const f32 dt, Particle& particle) override;

public:
    f32 drag = f32(0.0f);
};

//////////////////////////////////////////////////////////////////////////

class ParticleGravity : public ParticleUpdateProcess
{
    DEFINE_CLASS_UID(ParticleGravity)
public:
    virtual void Update(const f32 dt, Particle& particle) override;

public:
    float3 gravity = float3(0.0f, -9.81f, 0.0f);
};

//////////////////////////////////////////////////////////////////////////
