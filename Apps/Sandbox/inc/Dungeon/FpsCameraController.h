//////////////////////////////////////////////////////////////////////////
//    File        	: FpsCameraController.h
//    Created By    : Jack Spink
//    Created On 	: [1/11/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Object/GameObject.h"

//////////////////////////////////////////////////////////////////////////

class PerspectiveCamera;

//////////////////////////////////////////////////////////////////////////

class FpsCameraController : public GameObject
{
    DECLARE_OBJECT(FpsCameraController, GameObject)
public:
    virtual ~FpsCameraController() = default;

public:
    virtual void OnConstruct() override;
    virtual void OnUpdate(const f32 dt) override;

    inline void SetInputEnabled(const bool enabled) { inputEnabled = enabled; }

private:
    void HandleInput(const f32 dt);

private:
    bool inputEnabled = true;
    f32 eyeHeight = 1.25f;
    f32 gravityMovement = 0.0f;
    float3 velocity = float3(0.0f);
    Ref<PerspectiveCamera> camera = nullptr;    // not a component
};

//////////////////////////////////////////////////////////////////////////
