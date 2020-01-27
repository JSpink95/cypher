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

class CameraPerspective: public CameraBase
{
    DECLARE_DERIVED(CameraPerspective, CameraBase)
public:
    virtual fmat4 GetViewMatrix() const override;
    virtual fmat4 GetProjectionMatrix() const override;
    virtual fmat4 GetViewProjectionMatrix() const override;
    virtual float3 GetEyeLocation() const override;
    virtual float3 GetEyeDirection() const override;
    virtual void OnUpdate(const f32 dt) override;

public:
    void SetEyeAndDirection(const float3& e, const float3& d);
    void SetScreenDimensions(const float2& dimensions);

private:
    void RebuildMatrixCache();

public:
    f32 fovDegrees = 90.0f;
    f32 nearClip = 0.01f;
    f32 farClip = 100.0f;

private:
    float3 eye, direction;

private:
    fmat4 view, projection;
    fmat4 viewProjection;
    float2 screenDimensions = float2(1.0f);
};

//////////////////////////////////////////////////////////////////////////
