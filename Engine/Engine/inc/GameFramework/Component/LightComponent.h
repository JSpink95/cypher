//////////////////////////////////////////////////////////////////////////
//    File        	: LightComponent.h
//    Created By    : Jack Spink
//    Created On 	: [3/12/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Component/TransformComponent.h"

//////////////////////////////////////////////////////////////////////////

class LightObject;
class TransformComponent;

//////////////////////////////////////////////////////////////////////////

class LightComponent: public TransformComponent
{
    DECLARE_DERIVED_COMPONENT(LightComponent, TransformComponent)
public:
    LightComponent();

public:
    virtual void OnConstruct() override;
    virtual void OnDestruct() override;

    virtual void OnUpdate(const f32 dt) override;

    inline Ref<LightObject> GetLightInstance() { return light; }

    inline float3 GetTransformedLightPosition() const
    {
        return float3(CalculateTransformMatrix() * float4(0.0f, 0.0f, 0.0f, 1.0f));
    }

public:
    f32 radius = 16.0f;
    float3 color = float3(1.0f);

private:
    Ref<LightObject> light;
};

//////////////////////////////////////////////////////////////////////////
