//////////////////////////////////////////////////////////////////////////
//    File        	: Player.h
//    Created By    : Jack Spink
//    Created On 	: [11/12/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Object/Character.h"

//////////////////////////////////////////////////////////////////////////

class PerspectiveCamera;

//////////////////////////////////////////////////////////////////////////

class Player : public Character
{
    DECLARE_DERIVED_OBJECT(Player, Character)
public:

    virtual void OnConstruct() override;
    virtual void OnUpdate(const f32 dt) override;
    virtual void OnPhysicsUpdate(const f32 dt) override;

private:
    Ref<PerspectiveCamera> camera = nullptr;    // not a component
    float2 input = float2(0.0f);
    float3 cameraRotation = float3(0.0f);
    bool holdingObject = false;
};

//////////////////////////////////////////////////////////////////////////
