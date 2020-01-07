//////////////////////////////////////////////////////////////////////////
//    File        	: ParticleUpdater.h
//    Created By    : Jack Spink
//    Created On 	: [1/11/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"

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
public:
    virtual void Initialise(Particle& particle) = 0;
};

//////////////////////////////////////////////////////////////////////////

class ParticleUpdateProcess
{
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
public:
    virtual void Initialise(Particle& particle) override;

public:

    inline float3& GetPoint()
    {
        return point;
    }

    inline void SetPoint(const float3& newPoint)
    {
        point = newPoint;
    }

private:
    float3 point = float3(0.0f);
};

//////////////////////////////////////////////////////////////////////////

class ParticleSphereEmitter : public ParticleEmissionProcess
{
public:
    virtual void Initialise(Particle& particle) override;

public:

    inline void SetRadiusRange(const float2& newRadiusRange)
    {
        radius = newRadiusRange;
    }

    inline void SetClampToEdge(const bool newClampToEdge)
    {
        clampToEdge = newClampToEdge;
    }

private:
    float2 radius = float2(0.0f, 1.0f);
    float2 direction = float2(-1.0f, 1.0f);
    bool clampToEdge = false;
};

//////////////////////////////////////////////////////////////////////////

class ParticleSetLifetime : public ParticleEmissionProcess
{
public:
    virtual void Initialise(Particle& particle) override;

public:

    inline f32& GetLifetime()
    {
        return lifetime;
    }

    inline void SetLifetime(const f32 newLifetime)
    {
        lifetime = newLifetime;
    }

private:
    f32 lifetime = 1.0f;
};

//////////////////////////////////////////////////////////////////////////

class ParticleSetSize : public ParticleEmissionProcess
{
public:
    virtual void Initialise(Particle& particle) override;

public:
    float2 size = float2(1.0f);
};

class ParticleSetSizeRandom : public ParticleEmissionProcess
{
public:
    virtual void Initialise(Particle& particle) override;

    inline float2& GetMinSize()
    {
        return minSize;
    }

    inline float2& GetMaxSize()
    {
        return minSize;
    }

    inline void SetMinSize(const float2& newMinSize)
    {
        minSize = newMinSize;
    }

    inline void SetMaxSize(const float2& newMaxSize)
    {
        maxSize = newMaxSize;
    }

private:
    float2 minSize = float2(0.5f);
    float2 maxSize = float2(1.0f);
};

//////////////////////////////////////////////////////////////////////////

class ParticleSetUV : public ParticleEmissionProcess
{
public:
    virtual void Initialise(Particle& particle) override;

public:
    inline float2& GetScale()
    {
        return scale;
    }

    inline float2& GetOffset()
    {
        return offset;
    }

    inline void SetScale(const float2& newScale)
    {
        scale = newScale;
    }

    inline void SetOffset(const float2& newOffset)
    {
        offset = newOffset;
    }

private:
    float2 scale = float2(1.0f);
    float2 offset = float2(0.0f);
};

//////////////////////////////////////////////////////////////////////////

class ParticleSetVelocityRandom : public ParticleEmissionProcess
{
public:
    virtual void Apply(Particle& particle);
    inline void SetStrength(const float2& newStrength)
    {
        strength = newStrength;
    }

private:
    float2 strength = float2(0.0f, 1.0f);
    float2 xyzDirection[3] = { float2(-1.0f, 1.0f), float2(-1.0f, 1.0f), float2(-1.0f, 1.0f) };
};

//////////////////////////////////////////////////////////////////////////

class ParticleLinearDrag : public ParticleUpdateProcess
{
public:
    virtual void Update(const f32 dt, Particle& particle) override;

public:
    inline float3& GetDrag()
    {
        return drag;
    }

    inline void SetDrag(const float3& newDrag)
    {
        drag = newDrag;
    }

private:
    float3 drag = float3(0.0f);
};

//////////////////////////////////////////////////////////////////////////

class ParticleGravity : public ParticleUpdateProcess
{
public:
    virtual void Update(const f32 dt, Particle& particle) override;

public:
    inline float3& GetGravity()
    {
        return gravity;
    }

    inline void SetGravity(const float3& newGravity)
    {
        gravity = newGravity;
    }

private:
    float3 gravity = float3(0.0f, -9.81f, 0.0f);
};

//////////////////////////////////////////////////////////////////////////
