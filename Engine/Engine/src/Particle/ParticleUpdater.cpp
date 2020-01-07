//////////////////////////////////////////////////////////////////////////
//    File        	: ParticleUpdater.cpp
//    Created By    : Jack Spink
//    Created On 	: [1/11/2019]
//////////////////////////////////////////////////////////////////////////

#include "Particle/ParticleUpdater.h"
#include "Particle/ParticleSystem.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Utility/RandomUtils.h"

//////////////////////////////////////////////////////////////////////////

void ParticleEmissionStage::Initialise(Particle& particle)
{
    for (ProcessType process : processes)
    {
        if (process)
        {
            process->Initialise(particle);
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void ParticleUpdateStage::Update(const f32 dt, Particle& particle)
{
    for (ProcessType process : processes)
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
    particle.age = global_random::as_float(lifetime.x, lifetime.y);
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
