//////////////////////////////////////////////////////////////////////////
//    File        	: PerspectiveCameraComponent.h
//    Created By    : Jack Spink
//    Created On 	: [07/02/2020]
//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Component/PerspectiveCameraComponent.h"
#include "GameFramework/Component/TransformComponent.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassManager.h"

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Camera/PerspectiveCamera.h"

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(PerspectiveCameraComponent, Component)
    RTTI_PROPERTY(PerspectiveCameraComponent, ComponentRef<TransformComponent>, transform)
    RTTI_PROPERTY(PerspectiveCameraComponent, Ref<CameraPerspective>, camera)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

PerspectiveCameraComponent::PerspectiveCameraComponent()
{
    // enable ticking
    tickFunction.enabled = true;

    // setup transform component ref
    transform.componentName = "RootTransform";

    // create persepctive camera
    camera = std::make_shared<CameraPerspective>();
    camera->fovDegrees = 60.0f;
    camera->SetScreenDimensions(RenderPassManager::GetFramebufferSize());
}

//////////////////////////////////////////////////////////////////////////

void PerspectiveCameraComponent::OnConstruct()
{
    Super::OnConstruct();

    transform.OnConstruct(owner);
}

//////////////////////////////////////////////////////////////////////////

void PerspectiveCameraComponent::OnTick(const f32 dt)
{
    Super::OnTick(dt);
    UpdateCamera();
}

//////////////////////////////////////////////////////////////////////////

void PerspectiveCameraComponent::SetAsMainCamera()
{
    CameraBase::SetActiveCamera(camera.get());
}

//////////////////////////////////////////////////////////////////////////

void PerspectiveCameraComponent::SetTarget(const float3& newTarget)
{
    target = newTarget;
    UpdateCamera();
}

//////////////////////////////////////////////////////////////////////////

void PerspectiveCameraComponent::UpdateCamera()
{
    if (transform && camera)
    {
        const float3 direction = (transform->position == target) ? float3(0.0f, 0.0f, 1.0f) : glm::normalize(target - transform->position);
        camera->SetEyeAndDirection(transform->position, direction);
    }
}

//////////////////////////////////////////////////////////////////////////
