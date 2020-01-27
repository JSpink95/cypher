//////////////////////////////////////////////////////////////////////////
//    File        	: Camera.h
//    Created By    : Jack Spink
//    Created On 	: [5/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/RTTI.h"
#include "Core/RTTI/RTTIObject.h"

//////////////////////////////////////////////////////////////////////////

class CameraBase : public RTTIObject
{
    DECLARE_DERIVED(CameraBase, RTTIObject)
public:
    virtual ~CameraBase() {}

public:
    virtual fmat4 GetViewMatrix() const { return fmat4(1.0f); };
    virtual fmat4 GetProjectionMatrix() const { return fmat4(1.0f); };
    virtual fmat4 GetViewProjectionMatrix() const { return fmat4(1.0f); };
    virtual float3 GetEyeLocation() const { return float3(0.0f); }
    virtual float3 GetEyeDirection() const { return float3(0.0f, 0.0f, -1.0f); }

public:
    virtual void OnUpdate(const f32 dt) {}
    virtual void MakeThisActive();

public:
    static inline CameraBase* GetActiveCamera() { return activeCameraInstance; }
    static inline void SetActiveCamera(CameraBase* camera) { activeCameraInstance = camera; }

private:
    static inline CameraBase* activeCameraInstance = nullptr;
};

//////////////////////////////////////////////////////////////////////////
