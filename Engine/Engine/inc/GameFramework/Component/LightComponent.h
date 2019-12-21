//////////////////////////////////////////////////////////////////////////
//    File        	: LightComponent.h
//    Created By    : Jack Spink
//    Created On 	: [3/12/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Component.h"

//////////////////////////////////////////////////////////////////////////

class LightObject;
class TransformComponent;

//////////////////////////////////////////////////////////////////////////

class LightComponent: public Component
{
    DECLARE_DERIVED_COMPONENT(LightComponent, Component)
public:
    // {77081A1F-5AB0-4F85-9D0D-2F221FF44DB1}
    static inline const GUID ClassId = { 0x77081a1f, 0x5ab0, 0x4f85, { 0x9d, 0xd, 0x2f, 0x22, 0x1f, 0xf4, 0x4d, 0xb1 } };

public:
    virtual void OnConstruct() override;
    virtual void OnDestruct() override;

    virtual void OnUpdate(const f32 dt) override;

    inline Ref<LightObject> GetLightInstance() { return light; }

private:
    Ref<LightObject> light;
    WeakRef<TransformComponent> ownerTransform;
};

//////////////////////////////////////////////////////////////////////////
