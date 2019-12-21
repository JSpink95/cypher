//////////////////////////////////////////////////////////////////////////
//    File        	: Weapon.cpp
//    Created By    : Jack Spink
//    Created On 	: [20/11/2019]
//////////////////////////////////////////////////////////////////////////

#include "Dungeon/Weapon.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/ObjectManager.h"
#include "Core/Input/Input.h"
#include "Core/Thread/GameThread.h"

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Component/ProjectileMovementComponent.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass.h"

//////////////////////////////////////////////////////////////////////////

#include "Particle/ParticleSystem.h"
#include "Particle/ParticleEmitter.h"

void BulletProjectile::OnConstruct()
{
    Super::OnConstruct();

    movement = NewComponent<ProjectileMovementComponent>();
    trail = NewComponent<ParticleSystemComponent>();

    // setup trail data

    Ref<ParticleEmitterCone> coneEmitter = std::make_shared<ParticleEmitterCone>();
    coneEmitter->GetData().emitTimeSeconds = 0.1f;
    coneEmitter->GetData().ageRange = { 0.2f, 1.0f };
    coneEmitter->GetData().coneHeight = 5.0f;
    coneEmitter->GetData().coneRadiusRange = { 0.0f, 0.25f };
    coneEmitter->GetData().coneSectionRangeDegrees = { 0.0f, 360.0f };
    coneEmitter->GetData().initialVelocityRange = { -0.2f, 0.2f };

    trail->SetEmitter(coneEmitter);

    GetGameThread()->AddObject(self);
    GetGameThread()->PushThreadTask(this, &BulletProjectile::RenderTask_Initialise);

}

//////////////////////////////////////////////////////////////////////////

void BulletProjectile::OnUpdate(const f32 dt)
{
    Super::OnUpdate(dt);

    // tick tick tick... that's the sound of your life running out
    timeUntilDeath -= dt;

    if (timeUntilDeath <= 0.0f)
    {
        //GetObjectManager()->DeregisterObject(GetInstanceId());
    }
}

//////////////////////////////////////////////////////////////////////////

void BulletProjectile::Initialise(const f32 newTimeUntilDeath, const float3& direction, const f32 speed)
{
    timeUntilDeath = newTimeUntilDeath;
    if (movement != nullptr)
    {
        movement->Initialise(direction, speed);
    }
}

//////////////////////////////////////////////////////////////////////////

void BulletProjectile::RenderTask_Initialise()
{
    RenderPassManager::AddObjectToPass(RenderPassType::Particle, self.lock());
}

//////////////////////////////////////////////////////////////////////////

void Weapon::OnConstruct()
{
    Super::OnConstruct();

    SetTickEnabled(true);
}

//////////////////////////////////////////////////////////////////////////

void Weapon::OnUpdate(const f32 dt)
{
    Super::OnUpdate(dt);

    // check for weapon fire
    if (owner != nullptr && InputManager::GetInputState("Shoot") == KeyboardState::Pressed)
    {
        float3 const facingDirection = fquat(glm::radians(owner->transform->rotation)) * float3(0.0f, 0.0f, 1.0f);

        Ref<BulletProjectile> newBullet = CreateObject<BulletProjectile>();
        newBullet->transform->position = owner->transform->position + float3(0.0f, 1.25f, 0.0f);
        newBullet->Initialise(5.0f, glm::normalize(facingDirection), 10.0f);
    }
}

//////////////////////////////////////////////////////////////////////////
