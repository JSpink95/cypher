//////////////////////////////////////////////////////////////////////////
//    File        	: TransformComponent.h
//    Created By    : Jack Spink
//    Created On 	: [18/10/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Component.h"

//////////////////////////////////////////////////////////////////////////

class TransformComponent : public Component
{
    DECLARE_DERIVED_COMPONENT(TransformComponent, Component)
public:
    float3 position = float3(0.0f);    // 
    float3 rotation = float3(0.0f);    // rotation is in euler angles (degrees)
    float3 scale    = float3(1.0f);    // 

public:
    virtual fmat4 CalculateTransformMatrix() const;

public:
    static Ref<Component> Create(Ref<Object> owner, pugi::xml_node node);
};

//////////////////////////////////////////////////////////////////////////
