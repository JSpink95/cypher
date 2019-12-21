//////////////////////////////////////////////////////////////////////////
//    File        	: ParticleEmitter.h
//    Created By    : Jack Spink
//    Created On 	: [1/11/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"

//////////////////////////////////////////////////////////////////////////

class ParticleSystemComponent;

//////////////////////////////////////////////////////////////////////////

struct FloatRange
{
    f32 min, max;

    f32 GetRandomFloat() const;
};

//////////////////////////////////////////////////////////////////////////

class ParticleEmitter
{
public:
    virtual void Tick(const f32 dt, ParticleSystemComponent* component) = 0;
};

//////////////////////////////////////////////////////////////////////////

class ParticleEmitterCone: public ParticleEmitter
{
public:
    struct Data
    {
        f32 emitTimeSeconds = 0.05f;
        f32 coneHeight = 2.0f;
        FloatRange ageRange = { 0.5f, 1.0f };
        FloatRange initialVelocityRange = { 0.5f, 1.5f };
        FloatRange coneRadiusRange = { 0.5f, 1.0f };
        FloatRange coneSectionRangeDegrees = { 0.0f, 180.0f };
    };

public:
    virtual void Tick(const f32 dt, ParticleSystemComponent* component) override;

    inline Data& GetData() { return data; }
    inline const Data& GetData() const { return data; }

private:
    Data data;
    f32 timeSinceLastEmission = 0.0f;
};

//////////////////////////////////////////////////////////////////////////
