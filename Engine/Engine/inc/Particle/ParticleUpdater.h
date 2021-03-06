//////////////////////////////////////////////////////////////////////////
//    File        	: ParticleUpdater.h
//    Created By    : Jack Spink
//    Created On 	: [1/11/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"
#include "Core/ObjectMacros.h"
#include "Core/RTTI/RTTIObject.h"

//////////////////////////////////////////////////////////////////////////

#include <random>
#include <vector>

//////////////////////////////////////////////////////////////////////////

struct Particle;

//////////////////////////////////////////////////////////////////////////

class ParticleEvent: public RTTIObject
{
    DECLARE_DERIVED(ParticleEvent, RTTIObject)
public:
    virtual void OnEvent(Particle& particle) {};
};

//////////////////////////////////////////////////////////////////////////

class ParticleProcess: public RTTIObject
{
    DECLARE_DERIVED(ParticleProcess, RTTIObject)
public:
    virtual ~ParticleProcess() {}
};

//////////////////////////////////////////////////////////////////////////

class ParticleEmissionProcess: public ParticleProcess
{
    DECLARE_DERIVED(ParticleEmissionProcess, ParticleProcess)
public:
    virtual void Initialise(Particle& particle) {};
};

//////////////////////////////////////////////////////////////////////////

class ParticleUpdateProcess: public ParticleProcess
{
    DECLARE_DERIVED(ParticleUpdateProcess, ParticleProcess)
public:
    virtual void Update(const f32 dt, Particle& particle) {};
};

//////////////////////////////////////////////////////////////////////////

class ParticleStage: public RTTIObject
{
    DECLARE_DERIVED(ParticleStage, RTTIObject)
public:
    virtual void AddOutput(Ref<ParticleProcess> process) {};

public:

    template<typename T>
    inline Ref<T> PushOutput()
    {
        Ref<T> output = std::make_shared<T>();

        if (output)
        {
            AddOutput(output);
        }

        return output;
    }
};

//////////////////////////////////////////////////////////////////////////

class ParticleEmissionStage: public ParticleStage
{
    DECLARE_DERIVED(ParticleEmissionStage, ParticleStage)
public:
    ParticleEmissionStage();

public:
    void Initialise(Particle& particle);
    virtual void AddOutput(Ref<ParticleProcess> process) override;

public:
    std::vector<Ref<ParticleEmissionProcess>> processes;
};

//////////////////////////////////////////////////////////////////////////

class ParticleUpdateStage: public ParticleStage
{
    DECLARE_DERIVED(ParticleUpdateStage, ParticleStage)
public:
    ParticleUpdateStage();

public:
    void Update(const f32 dt, Particle& particle);
    virtual void AddOutput(Ref<ParticleProcess> process) override;

public:
    std::vector<Ref<ParticleUpdateProcess>> processes;
};

//////////////////////////////////////////////////////////////////////////

class ParticlePointEmitter : public ParticleEmissionProcess
{
    DECLARE_DERIVED(ParticlePointEmitter, ParticleEmissionProcess)
public:
    virtual void Initialise(Particle& particle) override;

public:
    float3 point = float3(0.0f);
};

//////////////////////////////////////////////////////////////////////////

class ParticleSphereEmitter : public ParticleEmissionProcess
{
    DECLARE_DERIVED(ParticleSphereEmitter, ParticleEmissionProcess)
public:
    virtual void Initialise(Particle& particle) override;

public:
    float2 radius = float2(0.0f, 1.0f);
    float2 direction = float2(-1.0f, 1.0f);
    bool clampToEdge = false;
};

//////////////////////////////////////////////////////////////////////////

class ParticleLineEmitter : public ParticleEmissionProcess
{
    DECLARE_DERIVED(ParticleLineEmitter, ParticleEmissionProcess)
public:
    virtual void Initialise(Particle& particle) override;

public:
    float3 start = float3(0.0f);
    float3 end = float3(1.0f);

    bool sequenced = false;
    s32 maxPoints = 5;

private:
    s32 emittedPoints = 0;
};

//////////////////////////////////////////////////////////////////////////

class ParticleSetLifetime : public ParticleEmissionProcess
{
    DECLARE_DERIVED(ParticleSetLifetime, ParticleEmissionProcess)
public:
    virtual void Initialise(Particle& particle) override;

public:
    f32 lifetime = 1.0f;
};

class ParticleSetLifetimeRandom : public ParticleEmissionProcess
{
    DECLARE_DERIVED(ParticleSetLifetimeRandom, ParticleEmissionProcess)
public:
    virtual void Initialise(Particle& particle) override;

public:
    f32 minLifetime = 0.5f;
    f32 maxLifetime = 1.0f;
};

//////////////////////////////////////////////////////////////////////////

class ParticleSetSize : public ParticleEmissionProcess
{
    DECLARE_DERIVED(ParticleSetSize, ParticleEmissionProcess)
public:
    virtual void Initialise(Particle& particle) override;

public:
    float2 size = float2(1.0f);
};

class ParticleSetSizeRandom : public ParticleEmissionProcess
{
    DECLARE_DERIVED(ParticleSetSizeRandom, ParticleEmissionProcess)
public:
    virtual void Initialise(Particle& particle) override;

public:
    f32 minSize = 0.0f;
    f32 maxSize = 1.0f;
};

//////////////////////////////////////////////////////////////////////////

class ParticleSetUV : public ParticleEmissionProcess
{
    DECLARE_DERIVED(ParticleSetUV, ParticleEmissionProcess)
public:
    virtual void Initialise(Particle& particle) override;

public:
    float2 scale = float2(1.0f);
    float2 offset = float2(0.0f);
};

//////////////////////////////////////////////////////////////////////////

class ParticleSetVelocity : public ParticleEmissionProcess
{
    DECLARE_DERIVED(ParticleSetVelocity, ParticleEmissionProcess)
public:
    virtual void Initialise(Particle& particle) override;

public:
    f32 strength = 1.0f;
    float3 direction = float3(0.0f, 1.0f, 0.0f);
};

//////////////////////////////////////////////////////////////////////////

class ParticleSetVelocityRandom : public ParticleEmissionProcess
{
    DECLARE_DERIVED(ParticleSetVelocityRandom, ParticleEmissionProcess)
public:
    virtual void Initialise(Particle& particle) override;

public:
    float2 strength = float2(0.0f, 1.0f);
    float3 minDirections = float3(-1.0f);
    float3 maxDirections = float3(1.0f);
};

//////////////////////////////////////////////////////////////////////////

class ParticleSetColor : public ParticleEmissionProcess
{
    DECLARE_DERIVED(ParticleSetColor, ParticleEmissionProcess)
public:
    virtual void Initialise(Particle& particle) override;

public:
    float3 color = float3(1.0f);
    f32 emissiveStrength = 1.0f;
};

//////////////////////////////////////////////////////////////////////////

class ParticleLinearDrag : public ParticleUpdateProcess
{
    DECLARE_DERIVED(ParticleLinearDrag, ParticleUpdateProcess)
public:
    virtual void Update(const f32 dt, Particle& particle) override;

public:
    f32 drag = f32(0.0f);
};

//////////////////////////////////////////////////////////////////////////

class ParticleGravity : public ParticleUpdateProcess
{
    DECLARE_DERIVED(ParticleGravity, ParticleUpdateProcess)
public:
    virtual void Update(const f32 dt, Particle& particle) override;

public:
    float3 gravity = float3(0.0f, -9.81f, 0.0f);
};

//////////////////////////////////////////////////////////////////////////
