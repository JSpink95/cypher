//////////////////////////////////////////////////////////////////////////
//    File        	: SphereColliderComponent.cpp
//    Created By    : Jack Spink
//    Created On 	: [14/1/2020]
//////////////////////////////////////////////////////////////////////////

#include "Physics/Component/SphereColliderComponent.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/RTTI.h"

//////////////////////////////////////////////////////////////////////////

#include "Physics/Geometry/SphereCollisionShape.h"

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(SphereColliderComponent, ColliderComponent)
    RTTI_PROPERTY(SphereColliderComponent, f32, radius)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

void SphereColliderComponent::OnConstruct()
{
    Super::OnConstruct();

    collisionShape = std::make_shared<SphereCollision>(radius);
}

//////////////////////////////////////////////////////////////////////////
