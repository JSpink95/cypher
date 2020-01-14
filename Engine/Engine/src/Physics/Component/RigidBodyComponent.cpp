//////////////////////////////////////////////////////////////////////////
//    File        	: RigidBodyComponent.cpp
//    Created By    : Jack Spink
//    Created On 	: [23/12/2019]
//////////////////////////////////////////////////////////////////////////

#include "Physics/Component/RigidBodyComponent.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/RTTI.h"

//////////////////////////////////////////////////////////////////////////

#include "Physics/PhysicsWorld.h"

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Component/TransformComponent.h"

//////////////////////////////////////////////////////////////////////////

#include "BulletDynamics/Dynamics/btRigidBody.h"

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN(RigidBodyComponent)
    RTTI_PROPERTY(RigidBodyComponent, f32, mass)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

void RigidBodyComponent::OnConstruct()
{
    Super::OnConstruct();

    ownerTransform = owner->FindFirstComponentOfType<TransformComponent>();
}

//////////////////////////////////////////////////////////////////////////

void RigidBodyComponent::OnUpdate(const f32 dt)
{
    Super::OnUpdate(dt);

    if (Ref<TransformComponent> transform = ownerTransform.lock())
    {
        if (body != nullptr)
        {
            btTransform physicsTransform;
            body->getMotionState()->getWorldTransform(physicsTransform);

            const btVector3& physicsPosition = physicsTransform.getOrigin();
            btQuaternion physicsRotation = physicsTransform.getRotation();

            float3 eulerRotation;
            physicsRotation.getEulerZYX(eulerRotation.z, eulerRotation.y, eulerRotation.x);

            transform->position = float3(physicsPosition.x(), physicsPosition.y(), physicsPosition.z());
            transform->rotation = glm::degrees(eulerRotation);
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void RigidBodyComponent::Initialise(const f32 mass_deprecated)
{
    body = Physics::CreateRigidBody(mass, float3(0.0f), float3(0.0f), nullptr);
}

//////////////////////////////////////////////////////////////////////////
