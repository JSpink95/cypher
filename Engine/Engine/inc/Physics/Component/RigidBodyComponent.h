//////////////////////////////////////////////////////////////////////////
//    File        	: RigidBodyComponent.h
//    Created By    : Jack Spink
//    Created On 	: [23/12/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Component.h"
#include "Core/ComponentRef.h"

//////////////////////////////////////////////////////////////////////////

#include "pugixml.hpp"

//////////////////////////////////////////////////////////////////////////

class btRigidBody;
class ColliderComponent;
class TransformComponent;

//////////////////////////////////////////////////////////////////////////

class RigidBodyComponent : public Component
{
    DECLARE_COMPONENT(RigidBodyComponent, Component)
public:
    virtual void OnConstruct() override;
    virtual void OnTick(const f32 dt) override;

    void Initialise(const f32 mass_deprecated);

public:
    f32 mass = 100.0f;
    ComponentRef<ColliderComponent> attachedCollider;
    ComponentRef<TransformComponent> attachedTransform;

private:
    btRigidBody* body = nullptr;
};

//////////////////////////////////////////////////////////////////////////
