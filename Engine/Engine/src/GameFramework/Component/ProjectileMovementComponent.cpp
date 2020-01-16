//////////////////////////////////////////////////////////////////////////
//    File        	: ProjectileMovementComponent.cpp
//    Created By    : Jack Spink
//    Created On 	: [20/11/2019]
//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Component/ProjectileMovementComponent.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Object.h"
#include "Core/RTTI/RTTI.h"
#include "Core/Thread/GameThread.h"

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Component/TransformComponent.h"

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN(ProjectileMovementComponent)
    RTTI_PROPERTY(ProjectileMovementComponent, f32, speed)
    //RTTI_PROPERTY(ProjectileMovementComponent, float3, direction) // probably don't need to expose this to the RTTI system
RTTI_END()

//////////////////////////////////////////////////////////////////////////

void ProjectileMovementComponent::OnConstruct()
{
    Super::OnConstruct();

    transform = GetOwner()->FindComponentAsType<TransformComponent>("RootComponent");
}

//////////////////////////////////////////////////////////////////////////

void ProjectileMovementComponent::OnUpdate(const f32 dt)
{
    Super::OnUpdate(dt);

    if (transform != nullptr)
    {
        transform->position += (direction * speed * dt);
    }
}

//////////////////////////////////////////////////////////////////////////

void ProjectileMovementComponent::Initialise(const float3& newDirection, const f32 newSpeed)
{
    direction = newDirection;
    speed = newSpeed;
}

//////////////////////////////////////////////////////////////////////////
