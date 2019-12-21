//////////////////////////////////////////////////////////////////////////
//    File        	: PlaneCollisionShape.cpp
//    Created By    : Jack Spink
//    Created On 	: [3/12/2019]
//////////////////////////////////////////////////////////////////////////

#include "Physics/Geometry/PlaneCollisionShape.h"
#include "Physics/PhysicsWorld.h"

//////////////////////////////////////////////////////////////////////////

PlaneCollision::PlaneCollision(const float3& n, const f32 d)
    : normal(n)
    , distance(d)
{
    collision = PhysicsWorld::GetActiveInstance()->CreatePlane(normal, distance);
}

//////////////////////////////////////////////////////////////////////////
