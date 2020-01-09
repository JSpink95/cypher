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

//////////////////////////////////////////////////////////////////////////

void BulletProjectile::OnConstruct()
{
    Super::OnConstruct();

    movement = CreateComponent<ProjectileMovementComponent>("Movement");
    trail = CreateComponent<ParticleSystemComponent>("ParticleTrail");

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

        Ref<BulletProjectile> newBullet = CreateObject<BulletProjectile>(ObjectId::Create("Bullet"));
        newBullet->transform->position = owner->transform->position + float3(0.0f, 1.25f, 0.0f);
        newBullet->Initialise(5.0f, glm::normalize(facingDirection), 10.0f);
    }
}

//////////////////////////////////////////////////////////////////////////
