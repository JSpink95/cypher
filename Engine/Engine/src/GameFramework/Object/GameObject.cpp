//////////////////////////////////////////////////////////////////////////
//    File        	: GameObject.cpp
//    Created By    : Jack Spink
//    Created On 	: [18/10/2019]
//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Object/GameObject.h"
#include "GameFramework/Component/TransformComponent.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/RTTI.h"

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(GameObject, Object)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

void GameObject::OnConstruct()
{
    Super::OnConstruct();

    transform = CreateComponent<TransformComponent>("RootTransform");
}

//////////////////////////////////////////////////////////////////////////
