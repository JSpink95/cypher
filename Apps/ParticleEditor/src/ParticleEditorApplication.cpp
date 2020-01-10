//////////////////////////////////////////////////////////////////////////
//    File        	: ParticleEditorApplication.cpp
//    Created By    : Jack Spink
//    Created On 	: [08/01/2020]
//////////////////////////////////////////////////////////////////////////

#include "ParticleEditorApplication.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/ObjectManager.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Window.h"

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Object/GameObject.h"
#include "GameFramework/Camera/PerspectiveCamera.h"

//////////////////////////////////////////////////////////////////////////

#include "Particle/ParticleSystem.h"
#include "Particle/ParticleUpdater.h"

//////////////////////////////////////////////////////////////////////////

// #temporary - make this an engine class

class OrbitalCameraComponent : public Component
{
    DECLARE_DERIVED_COMPONENT(OrbitalCameraComponent, Component)
public:
    virtual void OnConstruct() override
    {
        Super::OnConstruct();

        camera = std::make_shared<PerspectiveCamera>();
        ownerTransform = owner->FindFirstComponentOfType<TransformComponent>();
    }

    virtual void OnUpdate(const f32 dt) override
    {
        Super::OnUpdate(dt);

        if (Ref<TransformComponent> transform = ownerTransform.lock())
        {
            camera->SetPosition(transform->position);
            camera->SetLookAt(orbitOrigin);
        }
    }

public:

    void SetAsMainCamera()
    {
        Camera::SetActiveCamera(camera);
    }

    void SetOrbitOrigin(const float3& newOrbitOrigin)
    {
        orbitOrigin = newOrbitOrigin;
    }

private:
    WeakRef<TransformComponent> ownerTransform;
    Ref<PerspectiveCamera> camera;

    float3 orbitOrigin = float3(0.0f);
};

//////////////////////////////////////////////////////////////////////////

Ref<GameObject> CreateDefaultParticleEffect(const std::string& id, u32 number)
{
    Ref<GameObject> effect = CreateObject<GameObject>(ObjectId::Create(id + "-" + std::to_string(number)));

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

void ParticleEditorApplication::OnRenderCreate()
{
    Application::OnRenderCreate();

    window->Recentre();
}

//////////////////////////////////////////////////////////////////////////

void ParticleEditorApplication::OnPostCreate()
{
    Application::OnPostCreate();

    cameraObject = CreateObject<GameObject>(ObjectId::Create("MainCamera"));

    Ref<OrbitalCameraComponent> camera = cameraObject->CreateComponent<OrbitalCameraComponent>("Cam");
    camera->SetAsMainCamera();
    camera->SetOrbitOrigin(float3(0.0f));

    // create an initial particle system
    AddNewDefaultEffect("particle-system", true);
}

//////////////////////////////////////////////////////////////////////////

void ParticleEditorApplication::OnPostUpdate()
{
    Application::OnPostUpdate();
}

//////////////////////////////////////////////////////////////////////////

void ParticleEditorApplication::AddNewDefaultEffect(const std::string& id, bool makeActive/* = false*/, const float3& atLocation/* = float3(0.0f)*/)
{
    Ref<GameObject> effect = CreateDefaultParticleEffect(id, editableParticleSystems.size());
    effect->transform->position = atLocation;
    editableParticleSystems.push_back(effect);

    if (makeActive)
    {
        activeParticleSystem = effect;
    }
}

//////////////////////////////////////////////////////////////////////////

void main()
{
    Application::Main<ParticleEditorApplication>();
}

//////////////////////////////////////////////////////////////////////////
