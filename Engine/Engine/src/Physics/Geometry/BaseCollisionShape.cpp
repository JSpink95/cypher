//////////////////////////////////////////////////////////////////////////
//    File        	: BaseCollisionShape.cpp
//    Created By    : Jack Spink
//    Created On 	: [3/12/2019]
//////////////////////////////////////////////////////////////////////////

#include "Physics/Geometry/BaseCollisionShape.h"
#include "Physics/PhysicsWorld.h"

//////////////////////////////////////////////////////////////////////////

BaseCollisionShape::~BaseCollisionShape()
{
    if (collision)
    {
        PhysicsWorld::GetActiveInstance()->FreeCollisionShape(collision);
    }
}

//////////////////////////////////////////////////////////////////////////
