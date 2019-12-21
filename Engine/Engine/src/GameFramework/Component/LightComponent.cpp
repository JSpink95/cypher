//////////////////////////////////////////////////////////////////////////
//    File        	: LightComponent.cpp
//    Created By    : Jack Spink
//    Created On 	: [3/12/2019]
//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Component/LightComponent.h"
#include "GameFramework/Component/TransformComponent.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Object.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Effects/LightManager.h"

//////////////////////////////////////////////////////////////////////////

void LightComponent::OnConstruct()
{
    Super::OnConstruct();

    light = GetLightManager()->InsertInstance(vec3(0.0f), 16.0f, vec4(1.0f));
    ownerTransform = GetOwner()->FindFirstComponentOfType<TransformComponent>();

    if (Ref<TransformComponent> transform = ownerTransform.lock())
    {
        light->SetPosition(transform->position);
    }
}

//////////////////////////////////////////////////////////////////////////

void LightComponent::OnDestruct()
{
    Super::OnDestruct();

    GetLightManager()->RemoveInstance(light);
    light = nullptr;
}

//////////////////////////////////////////////////////////////////////////

void LightComponent::OnUpdate(const f32 dt)
{
    Super::OnUpdate(dt);

    if (Ref<TransformComponent> transform = ownerTransform.lock())
    {
        light->SetPosition(transform->position);
    }
}

//////////////////////////////////////////////////////////////////////////
