//////////////////////////////////////////////////////////////////////////
//    File        	: ProjectileMovementComponent.h
//    Created By    : Jack Spink
//    Created On 	: [20/11/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Component.h"
#include "Core/ComponentRef.h"

//////////////////////////////////////////////////////////////////////////

class TransformComponent;

//////////////////////////////////////////////////////////////////////////

class ProjectileMovementComponent : public Component
{
    DECLARE_COMPONENT(ProjectileMovementComponent, Component)
public:
    virtual void OnConstruct() override;
    virtual void OnTick(const f32 dt) override;

    void Initialise(const float3& newDirection, const f32 newSpeed);

public:
    float3 direction;
    f32 speed;

private:
    ComponentRef<TransformComponent> attachedTransform;
};

//////////////////////////////////////////////////////////////////////////
