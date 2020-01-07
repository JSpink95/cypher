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

class ParticleSetLifetimeRandom : public ParticleEmissionProcess
{
public:
    virtual void Initialise(Particle& particle) override;

public:

    inline f32& GetLifetimeMin()
    {
        return lifetime.x;
    }

    inline f32& GetLifetimeMax()
    {
        return lifetime.y;
    }

    inline void SetLifetimeMin(f32 min)
    {
        lifetime.x = min;
    }

    inline void SetLifetimeMax(f32 max)
    {
        lifetime.y = max;
    }


    inline void SetRange(const f32 min, const f32 max)
    {
        lifetime.x = min;
        lifetime.y = max;
    }

private:
    float2 lifetime;
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

    inline f32& GetMinSize()
    {
        return minSize;
    }

    inline f32& GetMaxSize()
    {
        return maxSize;
    }

    inline void SetMinSize(const f32 newMinSize)
    {
        minSize = newMinSize;
    }

    inline void SetMaxSize(const f32 newMaxSize)
    {
        maxSize = newMaxSize;
    }

private:
    f32 minSize = 0.0f;
    f32 maxSize = 1.0f;
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

class ParticleSetVelocity : public ParticleEmissionProcess
{
public:
    virtual void Initialise(Particle& particle) override;

    inline void SetStrength(const f32 newStrength)
    {
        strength = newStrength;
    }

    inline void SetDirection(const float3& newDirection)
    {
        direction = glm::normalize(newDirection);
    }

private:
    f32 strength = 1.0f;
    float3 direction = float3(0.0f, 1.0f, 0.0f);
};

//////////////////////////////////////////////////////////////////////////

class ParticleSetVelocityRandom : public ParticleEmissionProcess
{
public:
    virtual void Initialise(Particle& particle) override;

    inline void SetStrength(const float2& newStrength)
    {
        strength = newStrength;
    }

    inline void SetMinDirection(const float3& min)
    {
        minDirections = min;
    }

    inline void SetMaxDirection(const float3& max)
    {
        maxDirections = max;
    }

private:
    float2 strength = float2(0.0f, 1.0f);
    float3 minDirections = float3(-1.0f);
    float3 maxDirections = float3(1.0f);
};

//////////////////////////////////////////////////////////////////////////

class ParticleLinearDrag : public ParticleUpdateProcess
{
public:
    virtual void Update(const f32 dt, Particle& particle) override;

public:
    inline f32& GetDrag()
    {
        return drag;
    }

    inline void SetDrag(const f32 newDrag)
    {
        drag = newDrag;
    }

private:
    f32 drag = f32(0.0f);
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
