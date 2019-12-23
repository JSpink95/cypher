//////////////////////////////////////////////////////////////////////////
//    File        	: RigidBodyComponent.h
//    Created By    : Jack Spink
//    Created On 	: [23/12/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"
#include "Core/Component.h"

//////////////////////////////////////////////////////////////////////////

#include "pugixml.hpp"

//////////////////////////////////////////////////////////////////////////

class btRigidBody;
class TransformComponent;

//////////////////////////////////////////////////////////////////////////

class RigidBodyComponent : public Component
{
    DECLARE_DERIVED_COMPONENT(RigidBodyComponent, Component)
public:
    // {9F70F90A-AC21-4239-899B-35E539BCE3C7}
    static inline const GUID ClassId = { 0x9f70f90a, 0xac21, 0x4239, { 0x89, 0x9b, 0x35, 0xe5, 0x39, 0xbc, 0xe3, 0xc7 } };

    virtual void OnConstruct() override;
    virtual void OnUpdate(const f32 dt) override;

    void Initialise(const f32 mass);

private:
    btRigidBody* body = nullptr;
    WeakRef<TransformComponent> ownerTransform;

public:
    // #test - "deserialisation"
    static Ref<RigidBodyComponent> FromXmlNode(pugi::xml_node node);
};

//////////////////////////////////////////////////////////////////////////
