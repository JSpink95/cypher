//////////////////////////////////////////////////////////////////////////
//    File        	: ParticleEditorApplication.cpp
//    Created By    : Jack Spink
//    Created On 	: [08/01/2020]
//////////////////////////////////////////////////////////////////////////

#include "ParticleEditorApplication.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/ObjectManager.h"

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Object/GameObject.h"

//////////////////////////////////////////////////////////////////////////

#include "Particle/ParticleSystem.h"
#include "Particle/ParticleUpdater.h"

//////////////////////////////////////////////////////////////////////////

Ref<GameObject> CreateDefaultParticleEffect(const std::string& id)
{
    Ref<GameObject> effect = CreateObject<GameObject>(ObjectId::Create(id));

    Ref<ParticleSystemComponent> system = effect->CreateComponent<ParticleSystemComponent>("ParticleSystem");
    system->SetEmissionRate(0.005f);
    system->SetMaxParticlesAlive(1024);
    
    Ref<ParticlePointEmitter> emitter = system->GetEmissionStage()->PushOutput<ParticlePointEmitter>();
    emitter->SetPoint(float3(0.0f));

    Ref<ParticleSetLifetimeRandom> lifetime = system->GetEmissionStage()->PushOutput<ParticleSetLifetimeRandom>();
    lifetime->SetLifetimeMin(0.5f);
    lifetime->SetLifetimeMax(1.0f);

    Ref<ParticleSetVelocityRandom> velocity = system->GetEmissionStage()->PushOutput<ParticleSetVelocityRandom>();
    velocity->SetMinDirection(float3(-0.1f, 1.0f, -0.1f));
    velocity->SetMaxDirection(float3( 0.1f, 1.0f,  0.1f));
    velocity->SetStrength(float2(5.0f, 10.0f));

    Ref<ParticleGravity> gravity = system->GetUpdateStage()->PushOutput<ParticleGravity>();

    return effect;
}

//////////////////////////////////////////////////////////////////////////

void ParticleEditorApplication::OnPostCreate()
{
    Application::OnPostCreate();
}

//////////////////////////////////////////////////////////////////////////

void ParticleEditorApplication::OnPostUpdate()
{
    Application::OnPostUpdate();
}

//////////////////////////////////////////////////////////////////////////

void main()
{
    Application::Main<ParticleEditorApplication>();
}

//////////////////////////////////////////////////////////////////////////
