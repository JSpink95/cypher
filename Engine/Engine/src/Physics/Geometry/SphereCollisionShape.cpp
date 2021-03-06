//////////////////////////////////////////////////////////////////////////
//    File        	: SphereCollisionShape.cpp
//    Created By    : Jack Spink
//    Created On 	: [3/12/2019]
//////////////////////////////////////////////////////////////////////////

#include "Physics/Geometry/SphereCollisionShape.h"
#include "Physics/PhysicsWorld.h"

//////////////////////////////////////////////////////////////////////////

SphereCollision::SphereCollision(const f32 r)
    : radius(r)
{
    collision = PhysicsWorld::GetActiveInstance()->CreateSphere(radius);
}

//////////////////////////////////////////////////////////////////////////
