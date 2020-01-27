//////////////////////////////////////////////////////////////////////////
//    File        	: CameraManager.h
//    Created By    : Jack Spink
//    Created On 	: [16/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"

//////////////////////////////////////////////////////////////////////////

#include <array>
#include <vector>

//////////////////////////////////////////////////////////////////////////
// forward declarations

class CameraBase;

//////////////////////////////////////////////////////////////////////////

struct OscillationData
{
    f32 amplitude = 0.0f;
    f32 frequency = 0.0f;
};

//////////////////////////////////////////////////////////////////////////

struct CameraShakeData
{
    bool looping = false;
    f32 oscillationDuration = 1.0f;
    std::array<OscillationData, 3u> positionalOscillation;
    std::array<OscillationData, 3u> rotationalOscillation;
};

//////////////////////////////////////////////////////////////////////////

class CameraShake
{
public:
    CameraShake() = default;
    CameraShake(const Ref<CameraShakeData>& newData);

    f32 elapsedTime;
    CameraShakeData data;

    void Update(const f32 dt);

public:
    inline bool HasElapsed() const
    {
        return glm::abs(elapsedTime - data.oscillationDuration) <= glm::epsilon<f32>();
    }

    inline bool WantsToStop() const
    {
        return wantsToStop;
    }

    inline void ForceStop()
    {
        wantsToStop = true;
    }

    inline float3 GetPositionalOscillation() const
    {
        const f32 elapsedTimeAsDuration = (data.oscillationDuration == 0.0f) ? 1.0f : (elapsedTime / data.oscillationDuration);

        float3 result;
        for (u32 axis = 0u; axis < 3u; ++axis)
        {
            result[axis] = glm::sin(data.positionalOscillation.at(axis).frequency * glm::pi<f32>() * elapsedTimeAsDuration);
            result[axis] *= data.positionalOscillation.at(axis).amplitude;
        }

        return result;
    }

    inline float3 GetRotationalOscillation() const
    {
        const f32 elapsedTimeAsDuration = (data.oscillationDuration == 0.0f) ? 1.0f : (elapsedTime / data.oscillationDuration);

        float3 result;
        for (u32 axis = 0u; axis < 3u; ++axis)
        {
            result[axis] = glm::sin(data.rotationalOscillation.at(axis).frequency * glm::pi<f32>() * elapsedTimeAsDuration);
            result[axis] *= data.rotationalOscillation.at(axis).amplitude;
        }

        return result;
    }

private:
    bool wantsToStop = false;
};

//////////////////////////////////////////////////////////////////////////

class CameraManager
{
public:
    CameraManager();

    fmat4 GetViewMatrix() const;
    fmat4 GetViewProjectionMatrix() const;

    inline void SetActiveCamera(const Ref<CameraBase>& cam) { activeCamera = cam; }

    void Update();
    void PerformShake(const Ref<CameraShakeData>& shakeData);

private:

    fmat4 shakeMatrix = fmat4(1.0f);
    Ref<CameraBase> activeCamera;

    float3 positionOffset;
    std::vector<CameraShake> shakes;
};

//////////////////////////////////////////////////////////////////////////
