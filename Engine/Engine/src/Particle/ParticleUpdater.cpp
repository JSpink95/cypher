//////////////////////////////////////////////////////////////////////////
//    File        	: ParticleUpdater.cpp
//    Created By    : Jack Spink
//    Created On 	: [1/11/2019]
//////////////////////////////////////////////////////////////////////////

#include "Particle/ParticleUpdater.h"
#include "Particle/ParticleSystem.h"

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

void ParticleSetSize::Initialise(Particle& particle)
{
    particle.data.size = size;
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
    f32 s = strengthDistribution(engine);
    float3 direction = float3(
        xyzDirectionDistribution[0](engine),
        xyzDirectionDistribution[1](engine),
        xyzDirectionDistribution[2](engine)
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
