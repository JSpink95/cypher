//////////////////////////////////////////////////////////////////////////
//    File        	: BoxColliderComponent.cpp
//    Created By    : Jack Spink
//    Created On 	: [14/1/2020]
//////////////////////////////////////////////////////////////////////////

#include "Physics/Component/BoxColliderComponent.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/RTTI.h"

//////////////////////////////////////////////////////////////////////////

#include "Physics/Geometry/BoxCollisionShape.h"

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN(BoxColliderComponent)
    RTTI_PROPERTY(BoxColliderComponent, float3, halfSize)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

void BoxColliderComponent::OnConstruct()
{
    Super::OnConstruct();

    collisionShape = std::make_shared<BoxCollision>(halfSize);
}

//////////////////////////////////////////////////////////////////////////
