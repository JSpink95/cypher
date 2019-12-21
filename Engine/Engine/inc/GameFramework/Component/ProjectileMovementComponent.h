//////////////////////////////////////////////////////////////////////////
//    File        	: ProjectileMovementComponent.h
//    Created By    : Jack Spink
//    Created On 	: [20/11/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Component.h"

//////////////////////////////////////////////////////////////////////////

class TransformComponent;

//////////////////////////////////////////////////////////////////////////

class ProjectileMovementComponent : public Component
{
    DECLARE_DERIVED_COMPONENT(ProjectileMovementComponent, Component)
public:
    // {F2EE0735-719C-43D2-85F0-1ED8CA12B8CE}
    static inline const GUID ClassId = { 0xf2ee0735, 0x719c, 0x43d2, { 0x85, 0xf0, 0x1e, 0xd8, 0xca, 0x12, 0xb8, 0xce } };

public:
    virtual void OnConstruct() override;
    virtual void OnUpdate(const f32 dt) override;

    void Initialise(const float3& newDirection, const f32 newSpeed);

private:
    float3 direction;
    f32 speed;
    Ref<TransformComponent> transform;
};

//////////////////////////////////////////////////////////////////////////
