//////////////////////////////////////////////////////////////////////////
//    File        	: PerspectiveCamera.h
//    Created By    : Jack Spink
//    Created On 	: [19/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Camera/Camera.h"

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
