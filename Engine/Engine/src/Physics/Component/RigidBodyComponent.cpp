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
#include "Physics/Component/ColliderComponent.h"
#include "Physics/Geometry/BaseCollisionShape.h"

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Component/TransformComponent.h"

//////////////////////////////////////////////////////////////////////////

#include "BulletDynamics/Dynamics/btRigidBody.h"

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(RigidBodyComponent, Component)
    RTTI_PROPERTY(RigidBodyComponent, f32, mass)
    RTTI_PROPERTY(RigidBodyComponent, ComponentRef<ColliderComponent>, attachedCollider)
    RTTI_PROPERTY(RigidBodyComponent, ComponentRef<TransformComponent>, attachedTransform)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

void RigidBodyComponent::OnConstruct()
{
    Super::OnConstruct();

    attachedCollider.componentName = "Collider";
    attachedTransform.componentName = "RootTransform";

    attachedCollider.OnConstruct(owner);
    attachedTransform.OnConstruct(owner);
}

//////////////////////////////////////////////////////////////////////////

void RigidBodyComponent::OnTick(const f32 dt)
{
    Super::OnTick(dt);

    if (attachedTransform)
    {
        Ref<TransformComponent> transform = attachedTransform.get();
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
    Ref<BaseCollisionShape> collision = attachedCollider ? attachedCollider->GetCollisionShape() : nullptr;
    Ref<TransformComponent> transform = attachedTransform ? attachedTransform.get() : nullptr;

    body = Physics::CreateRigidBody(
        mass,
        transform != nullptr ? transform->position : float3(0.0f),
        transform != nullptr ? transform->rotation : float3(0.0f),
        collision != nullptr ? collision->btShape() : nullptr
    );
}

//////////////////////////////////////////////////////////////////////////
