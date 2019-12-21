//////////////////////////////////////////////////////////////////////////
//    File        	: ParticleEmitter.cpp
//    Created By    : Jack Spink
//    Created On 	: [1/11/2019]
//////////////////////////////////////////////////////////////////////////

#include "Particle/ParticleEmitter.h"
#include "Particle/ParticleSystem.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Utility/RandomUtils.h"

//////////////////////////////////////////////////////////////////////////

f32 FloatRange::GetRandomFloat() const
{
    return GetRandomEngine()->GetRandomInRange(min, max);
}

//////////////////////////////////////////////////////////////////////////

void ParticleEmitterCone::Tick(const f32 dt, ParticleSystemComponent* component)
{
    timeSinceLastEmission += dt;
    if (timeSinceLastEmission >= data.emitTimeSeconds)
    {
        timeSinceLastEmission = 0.0f;

        f32 age = data.ageRange.GetRandomFloat();
        f32 directionAngle = glm::radians(data.coneSectionRangeDegrees.GetRandomFloat());
        f32 coneRadius = data.coneRadiusRange.GetRandomFloat();
        f32 initialVelocity = data.initialVelocityRange.GetRandomFloat();
        float3 direction = glm::normalize(float3(glm::cos(directionAngle) * coneRadius, data.coneHeight, glm::sin(directionAngle) * coneRadius)) * initialVelocity;

        fmat4 const emissionTransform = component->CalculateTransformMatrix();
        component->PushParticle(emissionTransform * float4(float3(0.0f), 1.0f), float2(0.1f), direction, age);
    }
}

//////////////////////////////////////////////////////////////////////////
