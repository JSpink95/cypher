//////////////////////////////////////////////////////////////////////////
//    File        	: ParticleEditorApplication.cpp
//    Created By    : Jack Spink
//    Created On 	: [08/01/2020]
//////////////////////////////////////////////////////////////////////////

#include "ParticleEditorApplication.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/ObjectManager.h"
#include "Core/Thread/GameThread.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Window.h"
#include "Render/Utility/MeshLibrary.h"
#include "Render/Utility/MaterialLibrary.h"

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Object/GameObject.h"
#include "GameFramework/Camera/PerspectiveCamera.h"
#include "GameFramework/Component/StaticMeshComponent.h"
#include "GameFramework/Component/LightComponent.h"

//////////////////////////////////////////////////////////////////////////

#include "Particle/ParticleSystem.h"
#include "Particle/ParticleUpdater.h"

//////////////////////////////////////////////////////////////////////////
// #temporary - make these engine classes
//////////////////////////////////////////////////////////////////////////

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

class EditorController : public Object
{
	DECLARE_DERIVED_OBJECT(EditorController, Object)
private:
	struct InputState
	{
		bool bRotateLeftPressed = false;
		bool bRotateRightPressed = false;
		bool bRotateUpPressed = false;
		bool bRotateDownPressed = false;
		bool bZoomInPressed = false;
		bool bZoomOutPressed = false;
	};

public:

	virtual void OnConstruct() override
	{
		Super::OnConstruct();

		transform = CreateComponent<TransformComponent>("Transform");
		camera = CreateComponent<OrbitalCameraComponent>("Camera");

		camera->SetAsMainCamera();

		GameThread::AddObject(self);
	}

	virtual void OnUpdate(const f32 dt) override
	{
		Super::OnUpdate(dt);

		input.bRotateLeftPressed = Input::IsKeyDown(KeyboardKey::LEFT);
		input.bRotateRightPressed = Input::IsKeyDown(KeyboardKey::RIGHT);
		input.bRotateUpPressed = Input::IsKeyDown(KeyboardKey::UP);
		input.bRotateDownPressed = Input::IsKeyDown(KeyboardKey::DOWN);
		input.bZoomInPressed = Input::IsKeyDown(KeyboardKey::E);
		input.bZoomOutPressed = Input::IsKeyDown(KeyboardKey::Q);

		if (input.bZoomInPressed)
			zoom = glm::max(0.5f, zoom - dt * zoomSpeed);

		if (input.bZoomOutPressed)
			zoom = glm::min(100.0f, zoom + dt * zoomSpeed);

		float2 movement = float2(0.0f);
		if (input.bRotateLeftPressed)
			movement.x -= rotSpeed;

		if (input.bRotateRightPressed)
			movement.x += rotSpeed;

		if (input.bRotateDownPressed)
			movement.y -= rotSpeed;

		if (input.bRotateUpPressed)
			movement.y += rotSpeed;

		transform->rotation.y += movement.x * dt;
		transform->rotation.x = glm::clamp(transform->rotation.x + movement.y * dt, -89.0f, 89.0f);

		const float3 direction = glm::quat(glm::radians(transform->rotation)) * float3(0.0f, 0.0f, 1.0f);
		transform->position = float3(0.0f) + direction * zoom;
	}

private:
	InputState input;

	f32 rotSpeed = 90.0f;
	f32 zoomSpeed = 5.0f;
	f32 zoom = 0.5f;

	Ref<TransformComponent> transform;
	Ref<OrbitalCameraComponent> camera;
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

    Ref<ParticleSetSizeRandom> size = system->GetEmissionStage()->PushOutput<ParticleSetSizeRandom>();
    size->SetMinSize(0.1f);
    size->SetMaxSize(0.2f);

    Ref<ParticleSetVelocityRandom> velocity = system->GetEmissionStage()->PushOutput<ParticleSetVelocityRandom>();
    velocity->SetMinDirection(float3(-0.3f, 1.0f, -0.3f));
    velocity->SetMaxDirection(float3( 0.3f, 1.0f,  0.3f));
    velocity->SetStrength(float2(5.0f, 10.0f));

    Ref<ParticleGravity> gravity = system->GetUpdateStage()->PushOutput<ParticleGravity>();

    return effect;
}

//////////////////////////////////////////////////////////////////////////

void ParticleEditorApplication::OnRenderCreate()
{
    Application::OnRenderCreate();
}

//////////////////////////////////////////////////////////////////////////

void ParticleEditorApplication::OnPostCreate()
{
    Application::OnPostCreate();
    
    window->Recentre();
    
	editorController = CreateObject<EditorController>(ObjectId::Create("EditorController"));

	lightObject = CreateObject<GameObject>(ObjectId::Create("MainLightSource"));
	lightObject->transform->position = float3(0.0f, 6.0f, 0.0f);

	Ref<LightComponent> light = lightObject->CreateComponent<LightComponent>("Light");

    gridObject = CreateObject<GameObject>(ObjectId::Create("Grid"));
    RenderPassManager::AddObjectToPass(RenderPassType::Opaque, gridObject);

    Ref<StaticMeshComponent> gridMesh = gridObject->CreateComponent<StaticMeshComponent>("StaticMesh");
    gridMesh->SetMaterial(MaterialLibrary::GetMaterial("mesh-lit-tex-checkerboard"));
    gridMesh->SetMesh(MeshLibrary::GetMesh("game:mesh-plane"));
    gridMesh->SetScale(float3(5.0f, 1.0f, 5.0f));

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

    GameThread::AddObject(effect);
    RenderPassManager::AddObjectToPass(RenderPassType::Particle, effect);

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
