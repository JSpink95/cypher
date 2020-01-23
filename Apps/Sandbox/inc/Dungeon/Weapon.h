//////////////////////////////////////////////////////////////////////////
//    File        	: Weapon.h
//    Created By    : Jack Spink
//    Created On 	: [20/11/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Object.h"
#include "GameFramework/Object/GameObject.h"

//////////////////////////////////////////////////////////////////////////

class ParticleSystemComponent;
class ProjectileMovementComponent;

//////////////////////////////////////////////////////////////////////////

class BulletProjectile : public GameObject
{
    DECLARE_OBJECT(BulletProjectile, GameObject)
public:
    virtual void OnConstruct() override;
    virtual void OnUpdate(const f32 dt) override;

    void Initialise(const f32 newTimeUntilDeath, const float3& direction, const f32 speed);

    inline const f32 GetTimeUntilDeath() const { return timeUntilDeath; }

private:
    f32 timeUntilDeath;
    Ref<ProjectileMovementComponent> movement;
    Ref<ParticleSystemComponent> trail;

    void RenderTask_Initialise();
};

//////////////////////////////////////////////////////////////////////////

class Weapon : public Object
{
    DECLARE_OBJECT(Weapon, Object)
public:
    virtual void OnConstruct() override;
    virtual void OnUpdate(const f32 dt) override;

    inline void SetOwner(GameObject* newOwner) { owner = newOwner; }

private:
    GameObject* owner = nullptr;
};

//////////////////////////////////////////////////////////////////////////
