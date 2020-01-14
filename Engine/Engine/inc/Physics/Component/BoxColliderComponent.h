//////////////////////////////////////////////////////////////////////////
//    File        	: BoxColliderComponent.h
//    Created By    : Jack Spink
//    Created On 	: [14/1/2020]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Physics/Component/ColliderComponent.h"

//////////////////////////////////////////////////////////////////////////

class BoxColliderComponent : public ColliderComponent
{
    DECLARE_DERIVED_COMPONENT(BoxColliderComponent, ColliderComponent)
public:
    virtual void OnConstruct() override;

public:
    float3 halfSize = float3(1.0f);
};

//////////////////////////////////////////////////////////////////////////
