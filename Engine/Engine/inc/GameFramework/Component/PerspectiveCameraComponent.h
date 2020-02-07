//////////////////////////////////////////////////////////////////////////
//    File        	: PerspectiveCameraComponent.h
//    Created By    : Jack Spink
//    Created On 	: [07/02/2020]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Component.h"
#include "Core/ComponentRef.h"

//////////////////////////////////////////////////////////////////////////

class CameraPerspective;
class TransformComponent;

//////////////////////////////////////////////////////////////////////////

class PerspectiveCameraComponent : public Component
{
    DECLARE_COMPONENT(PerspectiveCameraComponent, Component)
public:
    PerspectiveCameraComponent();

    virtual void OnConstruct() override;
    virtual void OnTick(f32 dt) override;

public:
    void SetAsMainCamera();
    void SetTarget(const float3& newTarget);

public:
    ComponentRef<TransformComponent> transform;
    Ref<CameraPerspective> camera;

private:
    float3 target;

private:
    void UpdateCamera();
};

//////////////////////////////////////////////////////////////////////////
