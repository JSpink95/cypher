//////////////////////////////////////////////////////////////////////////
//    File        	: TransformComponent.h
//    Created By    : Jack Spink
//    Created On 	: [18/10/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Component.h"
#include "Core/ComponentRef.h"

//////////////////////////////////////////////////////////////////////////

class TransformComponent : public Component
{
    DECLARE_COMPONENT(TransformComponent, Component)
public:
    virtual void OnConstruct() override;

public:
    float3 position = float3(0.0f);    // 
    float3 rotation = float3(0.0f);    // rotation is in euler angles (degrees)
    float3 scale    = float3(1.0f);    // 

public:
    ComponentRef<TransformComponent> parentTransform;

public:
    virtual fmat4 CalculateTransformMatrix() const;

public:
    static Ref<Component> Create(Ref<Object> owner, pugi::xml_node node);
    static Ref<Component> CreateComponent(Ref<Object> owner, const std::string& id, pugi::xml_node node);
};

//////////////////////////////////////////////////////////////////////////
