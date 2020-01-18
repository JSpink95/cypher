//////////////////////////////////////////////////////////////////////////
//    File        	: ParticleEditorApplication.cpp
//    Created By    : Jack Spink
//    Created On 	: [08/01/2020]
//////////////////////////////////////////////////////////////////////////

#include "ParticleEditorApplication.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/ObjectManager.h"
#include "Core/ComponentRef.h"
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

#include "imgui.h"

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
        attachedTransform.componentName = "RootTransform";
        attachedTransform.OnConstruct(owner);
    }

    virtual void OnUpdate(const f32 dt) override
    {
        Super::OnUpdate(dt);

        if (attachedTransform)
        {
            camera->SetPosition(attachedTransform->position);
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
    ComponentRef<TransformComponent> attachedTransform;
    Ref<PerspectiveCamera> camera;

    float3 orbitOrigin = float3(0.0f);
};

//////////////////////////////////////////////////////////////////////////

class EditorControllerComponent : public Component
{
    DECLARE_DERIVED_COMPONENT(EditorControllerComponent, Component)
private:
    struct CameraInput
    {
        bool bMouseDown = false;
        bool bMiddleMouseDown = false;
        bool bCtrlModifierDown = false;
    };

public:

    virtual void OnConstruct() override
    {
        Super::OnConstruct();

        attachedTransform.componentName = "RootTransform";
        attachedTransform.OnConstruct(owner);

        attachedCameraComponent.componentName = "OrbitalCamera";
        attachedCameraComponent.OnConstruct(owner);

        const float3 direction = glm::quat(glm::radians(attachedTransform->rotation)) * float3(0.0f, 0.0f, 1.0f);
        attachedTransform->position = float3(0.0f) + direction * amountOfZoom;
    }

    virtual void OnUpdate(const f32 dt)
    {
        Super::OnUpdate(dt);

        input.bMouseDown = Input::IsButtonDown(MouseButton::Right);
        input.bMiddleMouseDown = Input::IsButtonDown(MouseButton::Middle);
        input.bCtrlModifierDown = Input::IsKeyDown(KeyboardKey::LEFT_CONTROL);

        bool cameraUpdated = false;

        if (input.bMouseDown)
        {
            cameraUpdated = true;
            const float2 mouseDelta = Input::GetMouseDelta();

            if (input.bCtrlModifierDown)
            {
                // zoom
                const f32 zoomMovement = mouseDelta.y * zoomSpeed;
                amountOfZoom = glm::max(0.1f, amountOfZoom + zoomMovement * dt);
            }
            else
            {
                // rotate camera
                const float2 movement = mouseDelta * rotationSpeed * -1.0f;

                attachedTransform->rotation.y += movement.x * dt;
                attachedTransform->rotation.x = glm::clamp(attachedTransform->rotation.x + movement.y * dt, -89.0f, 89.0f);
            }
        }
        else if (input.bMiddleMouseDown)
        {
            cameraUpdated = true;
            const float2 mouseDelta = Input::GetMouseDelta();

            // move orbit location
            const float3 direction = -(glm::quat(glm::radians(attachedTransform->rotation)) * float3(0.0f, 0.0f, 1.0f));
            const float3 up = float3(0.0f, 1.0f, 0.0f);
            const float3 right = glm::cross(up, direction);

            const float3 orbitMovement = (up * mouseDelta.y + right * mouseDelta.x) * moveSpeed;
            orbitLocation += orbitMovement * dt;

        }

        if (cameraUpdated)
        {
            attachedCameraComponent->SetOrbitOrigin(orbitLocation);

            const float3 direction = glm::quat(glm::radians(attachedTransform->rotation)) * float3(0.0f, 0.0f, 1.0f);
            attachedTransform->position = orbitLocation + direction * amountOfZoom;
        }

    }

public:
    float2 rotationSpeed = float2(10.0f);
    f32 zoomSpeed = 2.0f;
    f32 moveSpeed = 1.0f;

private:
    CameraInput input;
    f32 amountOfZoom = 1.0f;
    float3 orbitLocation = float3(0.0f);
    ComponentRef<TransformComponent> attachedTransform;
    ComponentRef<OrbitalCameraComponent> attachedCameraComponent;
};

//////////////////////////////////////////////////////////////////////////

Ref<GameObject> CreateDefaultParticleEffect(const std::string& id, u32 number)
{
    Ref<GameObject> effect = CreateObject<GameObject>(ObjectId::Create(id + "-" + std::to_string(number)));

    Ref<ParticleSystemComponent> system = effect->CreateComponent<ParticleSystemComponent>("ParticleSystem");
    system->SetEmissionRate(0.005f);
    system->SetMaxParticlesAlive(1024);
    
    Ref<ParticlePointEmitter> emitter = system->GetEmissionStage()->PushOutput<ParticlePointEmitter>();
    emitter->point = float3(0.0f);

    Ref<ParticleSetLifetimeRandom> lifetime = system->GetEmissionStage()->PushOutput<ParticleSetLifetimeRandom>();
    lifetime->minLifetime = 0.5f;
    lifetime->maxLifetime = 1.0f;

    Ref<ParticleSetSizeRandom> size = system->GetEmissionStage()->PushOutput<ParticleSetSizeRandom>();
    size->minSize = 0.1f;
    size->maxSize = 0.2f;

    Ref<ParticleSetVelocityRandom> velocity = system->GetEmissionStage()->PushOutput<ParticleSetVelocityRandom>();
    velocity->minDirections = float3(-0.3f, 1.0f, -0.3f);
    velocity->maxDirections = float3( 0.3f, 1.0f,  0.3f);
    velocity->strength = float2(5.0f, 10.0f);

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
    //window->SetWindowPosition(int2(1920, 200));
    
	ak47Object = CreateObject<Object>(ObjectId::Create("AK47"));

	Ref<TransformComponent> ak47Transform = ak47Object->CreateComponent<TransformComponent>("RootTransform");

	Ref<StaticMeshComponent> ak47StaticMesh = ak47Object->CreateComponent<StaticMeshComponent>("StaticMesh");
	ak47StaticMesh->SetMaterial(MaterialLibrary::GetMaterial("assets:\\materials\\ak47-material.xml"));
	ak47StaticMesh->SetMesh(MeshLibrary::GetMesh("assets:\\models\\ak47.obj"));

	tableObject = CreateObject<Object>(ObjectId::Create("Table"));

	Ref<TransformComponent> tableTransform = tableObject->CreateComponent<TransformComponent>("RootTransform");

	Ref<StaticMeshComponent> tableStaticMesh = tableObject->CreateComponent<StaticMeshComponent>("StaticMesh");
	tableStaticMesh->SetMaterial(MaterialLibrary::GetMaterial("assets:\\materials\\table-material.xml"));
	tableStaticMesh->SetMesh(MeshLibrary::GetMesh("assets:\\models\\table.obj"));

	{
		RenderPassManager::AddObjectToPass(RenderPassType::Opaque, tableObject);
		RenderPassManager::AddObjectToPass(RenderPassType::Opaque, ak47Object);
	}

	editorController = CreateObject<GameObject>(ObjectId::Create("EditorController"));

    Ref<OrbitalCameraComponent> orbitalCamera = editorController->CreateComponent<OrbitalCameraComponent>("OrbitalCamera");
    orbitalCamera->SetAsMainCamera();

    Ref<EditorControllerComponent> controller = editorController->CreateComponent<EditorControllerComponent>("EditorController");

    {
        GameThread::AddObject(editorController);
    }

	lightObject = CreateObject<GameObject>(ObjectId::Create("MainLightSource"));
	lightObject->transform->position = float3(0.0f, 6.0f, 0.0f);

	Ref<LightComponent> light = lightObject->CreateComponent<LightComponent>("Light");

    gridObject = CreateObject<GameObject>(ObjectId::Create("Grid"));
    RenderPassManager::AddObjectToPass(RenderPassType::Opaque, gridObject);

    Ref<StaticMeshComponent> gridMesh = gridObject->CreateComponent<StaticMeshComponent>("StaticMesh");
    gridMesh->SetMaterial(MaterialLibrary::GetMaterial("assets:\\materials\\dev-material.xml"));
    gridMesh->SetMesh(MeshLibrary::GetMesh("game:mesh-plane"));
    gridMesh->scale = float3(5.0f, 1.0f, 5.0f);

    // create an initial particle system
    AddNewDefaultEffect("particle-system", true);
}

//////////////////////////////////////////////////////////////////////////

void ParticleEditorApplication::OnPostUpdate()
{
    Application::OnPostUpdate();
}

//////////////////////////////////////////////////////////////////////////

void ParticleEditorApplication::OnImGuiRender()
{
    Application::OnImGuiRender();

    ImGui::SetNextWindowPos(ImVec2(4, 8));
    if (ImGui::Begin("Active Particle System", (bool*)0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse))
    {
        if (activeParticleSystem)
        {
            ImGui::ShowObjectProperties(activeParticleSystem);
        }
        else
        {
            ImGui::Text("No particle system selected");
        }

        ImGui::End();
    }

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
