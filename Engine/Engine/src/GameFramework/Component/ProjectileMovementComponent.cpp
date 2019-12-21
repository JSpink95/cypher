//////////////////////////////////////////////////////////////////////////
//    File        	: ProjectileMovementComponent.cpp
//    Created By    : Jack Spink
//    Created On 	: [20/11/2019]
//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Component/ProjectileMovementComponent.h"

#include "Core/Object.h"
#include "GameFramework/Component/TransformComponent.h"
#include "Core/Thread/GameThread.h"

//////////////////////////////////////////////////////////////////////////

void ProjectileMovementComponent::OnConstruct()
{
    Super::OnConstruct();

    transform = GetOwner()->FindFirstComponentOfType<TransformComponent>();
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
