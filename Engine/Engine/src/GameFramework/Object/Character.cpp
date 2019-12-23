//////////////////////////////////////////////////////////////////////////
//    File        	: Character.cpp
//    Created By    : Jack Spink
//    Created On 	: [4/12/2019]
//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Object/Character.h"

//////////////////////////////////////////////////////////////////////////

#include "Physics/Geometry/CapsuleCollisionShape.h"

//////////////////////////////////////////////////////////////////////////

#include "BulletDynamics/Dynamics/btRigidBody.h"

//////////////////////////////////////////////////////////////////////////

void Character::OnConstruct()
{
    Super::OnConstruct();

    Ref<CapsuleCollision> capsule = std::make_shared<CapsuleCollision>(0.75f, 1.5f);
    Initialise(100.0f, float3(0.0f, 2.0f, 0.0f), float3(0.0f), capsule);

    if (body != nullptr)
    {
        body->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
    }
}

//////////////////////////////////////////////////////////////////////////

void Character::OnPhysicsUpdate(const f32 dt)
{
    Super::OnPhysicsUpdate(dt);

    // #todo - ground checks

    if (body != nullptr)
    {
        body->activate();

        // movement doesn't affect gravity velocity
        btVector3 velocity = body->getLinearVelocity();
        velocity.setX(movement.x);
        velocity.setZ(movement.z);

        body->setLinearVelocity(velocity);
    }

    // reset movement
    movement = float3(0.0f);
}

//////////////////////////////////////////////////////////////////////////

void Character::AddMovementInput(const float3& move, const f32 amount)
{
    movement += (move * amount);
}

//////////////////////////////////////////////////////////////////////////
