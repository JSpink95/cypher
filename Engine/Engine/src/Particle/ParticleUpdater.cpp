//////////////////////////////////////////////////////////////////////////
//    File        	: ParticleUpdater.cpp
//    Created By    : Jack Spink
//    Created On 	: [1/11/2019]
//////////////////////////////////////////////////////////////////////////

#include "Particle/ParticleUpdater.h"
#include "Particle/ParticleSystem.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/RTTI.h"
#include "Core/RTTI/RTTIObject.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Utility/RandomUtils.h"

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(ParticleEvent, RTTIObject)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(ParticleProcess, RTTIObject)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(ParticleEmissionProcess, ParticleProcess)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(ParticleUpdateProcess, ParticleProcess)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(ParticleStage, RTTIObject)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(ParticleEmissionStage, ParticleStage)
    RTTI_PROPERTY_LIST(ParticleEmissionStage, std::vector, Ref<ParticleEmissionProcess>, processes)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(ParticleUpdateStage, ParticleStage)
    RTTI_PROPERTY_LIST(ParticleUpdateStage, std::vector, Ref<ParticleUpdateProcess>, processes)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(ParticlePointEmitter, ParticleEmissionProcess)
    RTTI_PROPERTY(ParticlePointEmitter, float3, point)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(ParticleSphereEmitter, ParticleEmissionProcess)
    RTTI_PROPERTY(ParticleSphereEmitter, float2, radius)
    RTTI_PROPERTY(ParticleSphereEmitter, float2, direction)
    RTTI_PROPERTY(ParticleSphereEmitter, bool, clampToEdge)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(ParticleSetLifetime, ParticleEmissionProcess)
    RTTI_PROPERTY(ParticleSetLifetime, f32, lifetime)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(ParticleSetLifetimeRandom, ParticleEmissionProcess)
    RTTI_PROPERTY(ParticleSetLifetimeRandom, f32, minLifetime)
    RTTI_PROPERTY(ParticleSetLifetimeRandom, f32, maxLifetime)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(ParticleSetSize, ParticleEmissionProcess)
    RTTI_PROPERTY(ParticleSetSize, float2, size)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(ParticleSetSizeRandom, ParticleEmissionProcess)
    RTTI_PROPERTY(ParticleSetSizeRandom, f32, minSize)
    RTTI_PROPERTY(ParticleSetSizeRandom, f32, maxSize)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(ParticleSetUV, ParticleEmissionProcess)
    RTTI_PROPERTY(ParticleSetUV, float2, scale)
    RTTI_PROPERTY(ParticleSetUV, float2, offset)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(ParticleSetVelocity, ParticleEmissionProcess)
    RTTI_PROPERTY(ParticleSetVelocity, f32, strength)
    RTTI_PROPERTY(ParticleSetVelocity, float3, direction)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(ParticleSetVelocityRandom, ParticleEmissionProcess)
    RTTI_PROPERTY(ParticleSetVelocityRandom, float2, strength)
    RTTI_PROPERTY(ParticleSetVelocityRandom, float3, minDirections)
    RTTI_PROPERTY(ParticleSetVelocityRandom, float3, maxDirections)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(ParticleLinearDrag, ParticleUpdateProcess)
    RTTI_PROPERTY(ParticleLinearDrag, f32, drag)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(ParticleGravity, ParticleUpdateProcess)
    RTTI_PROPERTY(ParticleGravity, float3, gravity)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

void ParticleEmissionStage::AddOutput(Ref<ParticleProcess> process)
{
    if (Ref<ParticleEmissionProcess> emissionProcess = std::dynamic_pointer_cast<ParticleEmissionProcess>(process))
    {
        processes.push_back(emissionProcess);
    }
}

//////////////////////////////////////////////////////////////////////////

void ParticleEmissionStage::Initialise(Particle& particle)
{
    for (Ref<ParticleEmissionProcess> process : processes)
    {
        if (process)
        {
            process->Initialise(particle);
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void ParticleUpdateStage::AddOutput(Ref<ParticleProcess> process)
{
    if (Ref<ParticleUpdateProcess> updateProcess = std::dynamic_pointer_cast<ParticleUpdateProcess>(process))
    {
        processes.push_back(updateProcess);
    }
}

//////////////////////////////////////////////////////////////////////////

void ParticleUpdateStage::Update(const f32 dt, Particle& particle)
{
    for (Ref<ParticleUpdateProcess> process : processes)
    {
        if (process)
        {
            process->Update(dt, particle);
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void ParticlePointEmitter::Initialise(Particle& particle)
{
    particle.data.position = point;
}

//////////////////////////////////////////////////////////////////////////

void ParticleSphereEmitter::Initialise(Particle& particle)
{
    f32 r = clampToEdge ? radius.y : global_random::as_float(radius.x, radius.y);
    float3 d = float3(
        global_random::as_float(direction.x, direction.y),
        global_random::as_float(direction.x, direction.y),
        global_random::as_float(direction.x, direction.y)
    );

    particle.data.position = r * glm::normalize(d);
}

//////////////////////////////////////////////////////////////////////////

void ParticleSetLifetime::Initialise(Particle& particle)
{
    particle.age = lifetime;
}

//////////////////////////////////////////////////////////////////////////

void ParticleSetLifetimeRandom::Initialise(Particle& particle)
{
    particle.age = global_random::as_float(minLifetime, maxLifetime);
}

//////////////////////////////////////////////////////////////////////////

void ParticleSetSize::Initialise(Particle& particle)
{
    particle.data.size = size;
}

//////////////////////////////////////////////////////////////////////////

void ParticleSetSizeRandom::Initialise(Particle& particle)
{
    particle.data.size = float2(global_random::as_float(minSize, maxSize));
}

//////////////////////////////////////////////////////////////////////////

void ParticleSetUV::Initialise(Particle& particle)
{
    particle.data.uv_scale = scale;
    particle.data.uv_offset = offset;
}

//////////////////////////////////////////////////////////////////////////

void ParticleSetVelocity::Initialise(Particle& particle)
{
    particle.velocity = strength * direction;
}

//////////////////////////////////////////////////////////////////////////

void ParticleSetVelocityRandom::Initialise(Particle& particle)
{
    f32 s = global_random::as_float(strength.x, strength.y);
    float3 direction = float3(
        global_random::as_float(minDirections.x, maxDirections.x),
        global_random::as_float(minDirections.y, maxDirections.y),
        global_random::as_float(minDirections.z, maxDirections.z)
    );

    particle.velocity = s * glm::normalize(direction);
}

//////////////////////////////////////////////////////////////////////////

void ParticleLinearDrag::Update(const f32 dt, Particle& particle)
{
    particle.velocity = glm::mix(particle.velocity, float3(0.0f), dt * drag);
}

//////////////////////////////////////////////////////////////////////////

void ParticleGravity::Update(const f32 dt, Particle& particle)
{
    particle.velocity += gravity * dt;
}

//////////////////////////////////////////////////////////////////////////
