//////////////////////////////////////////////////////////////////////////
//    File        	: PerspectiveCamera.cpp
//    Created By    : Jack Spink
//    Created On 	: [19/9/2019]
//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Camera/PerspectiveCamera.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/RTTI.h"

//////////////////////////////////////////////////////////////////////////

void PerspectiveCamera::SetPosition(const float3& newPosition)
{
    eye = newPosition;
    RebuildMatrixCache();
}

//////////////////////////////////////////////////////////////////////////

void PerspectiveCamera::SetLookAt(const float3& newLookAt)
{
    lookAt = newLookAt;
    RebuildMatrixCache();
}

//////////////////////////////////////////////////////////////////////////

void PerspectiveCamera::SetLookDirection(const float3& newLookDir)
{
    lookAt = eye + newLookDir;
    RebuildMatrixCache();
}

//////////////////////////////////////////////////////////////////////////

void PerspectiveCamera::RebuildMatrixCache()
{
    cachedView = glm::lookAt(eye, lookAt, vec3(0.0f, 1.0f, 0.0f));
    cachedProjection = glm::perspectiveFov(glm::radians(60.0f), 1280.0f / 4.0f, 720.0f / 4.0f, 0.01f, 100.0f);
    cachedViewProjection = cachedProjection * cachedView;
}

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(CameraPerspective, RTTIObject)
    RTTI_PROPERTY(CameraPerspective, f32, fovDegrees)
    RTTI_PROPERTY(CameraPerspective, f32, nearClip)
    RTTI_PROPERTY(CameraPerspective, f32, farClip)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

void CameraPerspective::UpdateCamera()
{
    RebuildMatrixCache();
}

//////////////////////////////////////////////////////////////////////////

void CameraPerspective::SetScreenDimensions(const float2& dimensions)
{
    screenDimensions = dimensions;
    UpdateCamera();
}

//////////////////////////////////////////////////////////////////////////

void CameraPerspective::RebuildMatrixCache()
{
    view = fmat4(1.0f);
    projection = glm::perspectiveFov(glm::radians(fovDegrees), screenDimensions.x, screenDimensions.y, nearClip, farClip);
    viewProjection = projection * view;
}

//////////////////////////////////////////////////////////////////////////
