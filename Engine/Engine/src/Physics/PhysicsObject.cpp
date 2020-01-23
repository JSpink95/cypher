//////////////////////////////////////////////////////////////////////////
//    File        	: PhysicsObject.cpp
//    Created By    : Jack Spink
//    Created On 	: [29/11/2019]
//////////////////////////////////////////////////////////////////////////

#include "Physics/PhysicsObject.h"
#include "Physics/PhysicsWorld.h"
#include "Physics/Geometry/BaseCollisionShape.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Thread/GameThread.h"
#include "Core/Thread/PhysicsThread.h"

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Component/TransformComponent.h"

//////////////////////////////////////////////////////////////////////////

#include "LinearMath/btTransform.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"

//////////////////////////////////////////////////////////////////////////

void PhysicsObject::OnConstruct()
{
    Super::OnConstruct();

    //GetGameThread()->AddObject(self);
    //GetPhysicsThread()->AddObject(self);
}

//////////////////////////////////////////////////////////////////////////

void PhysicsObject::OnDestruct()
{
    Super::OnDestruct();
}

//////////////////////////////////////////////////////////////////////////

void PhysicsObject::OnUpdate(const f32 dt)
{
    Super::OnUpdate(dt);

    if (body != nullptr)
    {
        btTransform physicsTransform;
        body->getMotionState()->getWorldTransform(physicsTransform);

        const btVector3& physicsPosition = physicsTransform.getOrigin();
        btQuaternion physicsRotation = physicsTransform.getRotation();

        transform->position = float3(physicsPosition.x(), physicsPosition.y(), physicsPosition.z());

        float3 eulerRotation;
        physicsRotation.getEulerZYX(eulerRotation.z, eulerRotation.y, eulerRotation.x);
        transform->rotation = glm::degrees(eulerRotation);
    }
}

//////////////////////////////////////////////////////////////////////////

void PhysicsObject::OnPhysicsUpdate(const f32 dt)
{
    Super::OnPhysicsUpdate(dt);
}

//////////////////////////////////////////////////////////////////////////

void PhysicsObject::Initialise(f32 mass, const float3& position, const float3& rotation, Ref<BaseCollisionShape> col)
{
    Ref<PhysicsWorld> physics = PhysicsWorld::GetActiveInstance();

    if (body != nullptr)
    {
        btCollisionShape* shape = body->getCollisionShape();
        physics->FreeCollisionShape(shape);
        physics->FreeRigidBody(body);
    }

    collision = col;
    body = PhysicsWorld::GetActiveInstance()->CreateRigidBody(mass, position, rotation, collision->btShape());
    body->setUserPointer(this);

    if (mass == 0.0f)
        body->setFriction(1.0f);
}

//////////////////////////////////////////////////////////////////////////

void PhysicsObject::SetGravityEnabled(bool gravity)
{
}

//////////////////////////////////////////////////////////////////////////

void PhysicsObject::SetPositionAndRotation(const float3& position, const float3& euler)
{
    if (body != nullptr)
    {
        btTransform transform;
        transform.setOrigin(btVector3(position.x, position.y, position.z));

        body->getMotionState()->setWorldTransform(transform);
        //body->setWorldTransform(transform);
    }
}

//////////////////////////////////////////////////////////////////////////

void PhysicsObject::ApplyDirectImpulse(const float3& direction, const f32 strength)
{
    //Ref<PhysicsObject> locked_self = std::dynamic_pointer_cast<PhysicsObject>(self.lock());
    //PhysicsWorld::GetActiveInstance()->ApplyImpulseToObject(locked_self, direction, strength);
}

//////////////////////////////////////////////////////////////////////////
