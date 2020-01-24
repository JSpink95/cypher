//////////////////////////////////////////////////////////////////////////
//    File        	: PerspectiveCamera.h
//    Created By    : Jack Spink
//    Created On 	: [19/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Camera/Camera.h"
#include "Core/RTTI/RTTIObject.h"

//////////////////////////////////////////////////////////////////////////

class PerspectiveCamera : public Camera
{
public:
    virtual inline float3 GetEyePosition() const override final { return eye; }

    void SetPosition(const float3& newPosition);
    void SetLookAt(const float3& newLookAt);
    void SetLookDirection(const float3& newLookDir);

    inline const float3 GetLookDirection() const { return glm::normalize(lookAt - eye); }
private:
    void RebuildMatrixCache();

private:
    float3 eye, lookAt;
};

//////////////////////////////////////////////////////////////////////////

class CameraPerspective: public RTTIObject
{
    DECLARE_DERIVED(CameraPerspective, RTTIObject)
public:
    void UpdateCamera();
    void SetScreenDimensions(const float2& dimensions);

private:
    void RebuildMatrixCache();

public:
    f32 fovDegrees = 90.0f;
    f32 nearClip = 0.01f;
    f32 farClip = 100.0f;

private:
    fmat4 view, projection;
    fmat4 viewProjection;
    float2 screenDimensions = float2(1.0f);
};

//////////////////////////////////////////////////////////////////////////
