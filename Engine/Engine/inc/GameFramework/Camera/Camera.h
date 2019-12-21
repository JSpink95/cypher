//////////////////////////////////////////////////////////////////////////
//    File        	: Camera.h
//    Created By    : Jack Spink
//    Created On 	: [5/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"
#include "Core/Events/WindowEvent.h"

//////////////////////////////////////////////////////////////////////////

class Camera
{
public:
    virtual ~Camera() = default;

    inline const fmat4& GetViewMatrix() const { return cachedView; }
    inline const fmat4& GetProjectionMatrix() const { return cachedProjection; }
    inline const fmat4& GetViewProjectionMatrix() const { return cachedViewProjection; }

    virtual float3 GetEyePosition() const = 0;
    
protected:
    fmat4 cachedView, cachedProjection;
    fmat4 cachedViewProjection;

public:
    static inline Ref<Camera> GetActiveCamera() { return activeCamera; }
    static inline void SetActiveCamera(Ref<Camera> newCamera) { activeCamera = newCamera; }

private:
    static inline Ref<Camera> activeCamera = nullptr;
};

//////////////////////////////////////////////////////////////////////////
