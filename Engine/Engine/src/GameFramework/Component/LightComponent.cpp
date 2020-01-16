//////////////////////////////////////////////////////////////////////////
//    File        	: LightComponent.cpp
//    Created By    : Jack Spink
//    Created On 	: [3/12/2019]
//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Component/LightComponent.h"
#include "GameFramework/Component/TransformComponent.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Object.h"
#include "Core/RTTI/RTTI.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Effects/LightManager.h"

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN(LightComponent)
    RTTI_PROPERTY(LightComponent, f32, radius)
    RTTI_PROPERTY(LightComponent, float3, color)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

void LightComponent::OnConstruct()
{
    Super::OnConstruct();

    light = GetLightManager()->InsertInstance(vec3(0.0f), radius, vec4(color, 1.0f));
    ownerTransform = GetOwner()->FindComponentAsType<TransformComponent>("RootComponent");

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
        light->SetRadius(radius);
        light->SetColor(vec4(color, 1.0f));
        light->SetPosition(transform->position);
    }
}

//////////////////////////////////////////////////////////////////////////
