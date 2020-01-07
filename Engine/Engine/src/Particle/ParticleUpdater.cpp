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

void ParticleSetSize::Initialise(Particle& particle)
{
    particle.data.size = size;
}

//////////////////////////////////////////////////////////////////////////

void ParticleSetSizeRandom::Initialise(Particle& particle)
{
    particle.data.size.x = global_random::as_float(minSize.x, maxSize.x);
    particle.data.size.y = global_random::as_float(minSize.y, maxSize.y);
}

//////////////////////////////////////////////////////////////////////////

void ParticleSetUV::Initialise(Particle& particle)
{
    particle.data.uv_scale = scale;
    particle.data.uv_offset = offset;
}

//////////////////////////////////////////////////////////////////////////

void ParticleSetVelocityRandom::Apply(Particle& particle)
{
    f32 s = global_random::as_float(strength.x, strength.y);
    float3 direction = float3(
        global_random::as_float(xyzDirection[0].x, xyzDirection[0].y),
        global_random::as_float(xyzDirection[1].x, xyzDirection[1].y),
        global_random::as_float(xyzDirection[2].x, xyzDirection[2].y)
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
