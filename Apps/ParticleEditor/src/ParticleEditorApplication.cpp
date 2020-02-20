//////////////////////////////////////////////////////////////////////////
//    File        	: ParticleEditorApplication.cpp
//    Created By    : Jack Spink
//    Created On 	: [08/01/2020]
//////////////////////////////////////////////////////////////////////////

#include "ParticleEditorApplication.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/RTTI.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/ObjectManager.h"
#include "Core/ComponentRef.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Thread/GameThread.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Utility/TimeUtils.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Serialise/Serialiser.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Window.h"
#include "Render/Utility/MeshLibrary.h"
#include "Render/Utility/MaterialLibrary.h"
#include "Render/Utility/DebugMeshVertexGenerator.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassLit.h"
#include "Render/Platform/RenderPass/RenderPassParticle.h"
#include "Render/Platform/RenderPass/RenderPassManager.h"

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Object/GameObject.h"
#include "GameFramework/Camera/PerspectiveCamera.h"
#include "GameFramework/Component/StaticMeshComponent.h"
#include "GameFramework/Component/LightComponent.h"

//////////////////////////////////////////////////////////////////////////

#include "Particle/ParticleSystem.h"
#include "Particle/ParticleUpdater.h"

//////////////////////////////////////////////////////////////////////////

#include "Audio/Audio.h"

//////////////////////////////////////////////////////////////////////////

#include "imgui.h"

//////////////////////////////////////////////////////////////////////////

#ifdef _OPENGL
#include "glfw.h"
#endif

//////////////////////////////////////////////////////////////////////////
// #temporary - make these engine classes
//////////////////////////////////////////////////////////////////////////

class OrbitalCameraComponent : public TransformComponent
{
    DECLARE_COMPONENT(OrbitalCameraComponent, TransformComponent)
public:

    OrbitalCameraComponent()
    {
        parentTransform.componentName = "RootTransform";
    }

    virtual void OnConstruct() override
    {
        Super::OnConstruct();

        camera = std::make_shared<CameraPerspective>();
        camera->SetScreenDimensions(float2(RenderPassManager::GetFramebufferSize()));
    }

public:

    void SetAsMainCamera()
    {
        camera->MakeThisActive();
    }

    void UpdateCameraData()
    {
        const float3 eye = float3(CalculateTransformMatrix() * float4(0.0f, 0.0f, 0.0f, 1.0f));
        const float3 eyeDirection = glm::normalize(float3(parentTransform->CalculateTransformMatrix() * float4(0.0f, 0.0f, 0.0f, 1.0f)) - eye);
        camera->SetEyeAndDirection(eye, eyeDirection);
    }

private:
    Ref<CameraPerspective> camera;
};

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(OrbitalCameraComponent, TransformComponent)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

class EditorControllerComponent : public Component
{
    DECLARE_COMPONENT(EditorControllerComponent, Component)
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
    }

    virtual void OnTick(const f32 dt) override
    {
        Super::OnTick(dt);

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

                attachedCameraComponent->rotation.y += movement.x * dt;
                attachedCameraComponent->rotation.x = glm::clamp(attachedCameraComponent->rotation.x + movement.y * dt, -89.0f, 89.0f);
            }
        }
        else if (input.bMiddleMouseDown)
        {
            cameraUpdated = true;
            const float2 mouseDelta = Input::GetMouseDelta();

            // move orbit location
            const float3 direction = -(glm::quat(glm::radians(attachedCameraComponent->rotation)) * float3(0.0f, 0.0f, 1.0f));
            const float3 up = float3(0.0f, 1.0f, 0.0f);
            const float3 right = glm::cross(up, direction);

            const float3 orbitMovement = (up * mouseDelta.y + right * mouseDelta.x) * moveSpeed;
            attachedTransform->position += orbitMovement * dt;
            
        }

        if (cameraUpdated)
        {
            const float3 direction = glm::quat(glm::radians(attachedCameraComponent->rotation)) * float3(0.0f, 0.0f, 1.0f);
            attachedCameraComponent->position = direction * amountOfZoom;
            attachedCameraComponent->UpdateCameraData();
        }
    }

public:
    float2 rotationSpeed = float2(10.0f);
    f32 zoomSpeed = 2.0f;
    f32 moveSpeed = 1.0f;

private:
    CameraInput input;
    f32 amountOfZoom = 4.0f;

public:
    ComponentRef<TransformComponent> attachedTransform;
    ComponentRef<OrbitalCameraComponent> attachedCameraComponent;
};

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(EditorControllerComponent, Component)
    RTTI_PROPERTY(EditorControllerComponent, TransformComponent, attachedTransform)
    RTTI_PROPERTY(EditorControllerComponent, OrbitalCameraComponent, attachedCameraComponent)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

class RotatingObjectComponent : public Component
{
    DECLARE_COMPONENT(RotatingObjectComponent, Component)
public:
    RotatingObjectComponent()
    {
        attachedTransform.componentName = "RootTransform";
    }

public:
    virtual void OnConstruct() override
    {
        Super::OnConstruct();

        attachedTransform.OnConstruct(owner);
    }

    virtual void OnTick(const f32 dt) override
    {
        Super::OnTick(dt);

        if (attachedTransform)
        {
            attachedTransform->rotation.y += rotationDegreesPerSec * dt;
        }
    }

public:
    f32 rotationDegreesPerSec = 45.0f;
    ComponentRef<TransformComponent> attachedTransform;
};

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(RotatingObjectComponent, Component)
    RTTI_PROPERTY(RotatingObjectComponent, f32, rotationDegreesPerSec, Clamp=(0.0,360.0))
    RTTI_PROPERTY(RotatingObjectComponent, ComponentRef<TransformComponent>, attachedTransform)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

Ref<Object> CreateDefaultParticleEffect(const std::string& id, u32 number)
{
    Ref<Object> effect = CreateObject<Object>(ObjectId::Create(id + "-" + std::to_string(number)));

    Ref<ParticleSystemComponent> system = effect->CreateComponent<ParticleSystemComponent>("ParticleSystem");
    system->SetEmissionRate(0.005f);
    system->SetMaxParticlesAlive(1024);
    system->SetTickEnabled(true);
    system->SetMaterial(MaterialLibrary::GetMaterial("assets:\\materials\\particle-default.xml"));
    
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

    Ref<ParticleSetColor> color = system->GetEmissionStage()->PushOutput<ParticleSetColor>();
    color->color = float3(1.0f, 0.0f, 1.0f);
    color->emissiveStrength = 50.0f;

    Ref<ParticleGravity> gravity = system->GetUpdateStage()->PushOutput<ParticleGravity>();

    return effect;
}

//////////////////////////////////////////////////////////////////////////

void ParticleEditorApplication::OnRenderCreate()
{
    Application::OnRenderCreate();

    GlCall(glLineWidth(2.0f));
}

//////////////////////////////////////////////////////////////////////////

void ParticleEditorApplication::OnPostCreate()
{
    Application::OnPostCreate();
    
    window->Recentre();
    //window->SetWindowPosition(int2(1920, 200));

    gizmoObject = CreateObject<GameObject>(ObjectId::Create("Gizmo"));
    gizmoObject->transform->position = float3(0.0f, 0.0f, 0.0f);

    Ref<StaticMeshComponent> gizmoMeshComponent = gizmoObject->CreateComponent<StaticMeshComponent>("StaticMesh");
    gizmoMeshComponent->SetMaterial("assets:\\materials\\wireframe.xml");
    gizmoMeshComponent->SetMesh("engine:\\mesh\\wireframe-sphere");
    //gizmoMeshComponent->SetRenderPass();

	editorController = CreateObject<GameObject>(ObjectId::Create("EditorController"));

    Ref<OrbitalCameraComponent> orbitalCamera = editorController->CreateComponent<OrbitalCameraComponent>("OrbitalCamera");
    orbitalCamera->SetAsMainCamera();

    Ref<EditorControllerComponent> controller = editorController->CreateComponent<EditorControllerComponent>("EditorController");
    controller->SetTickEnabled(true);

    Ref<StaticMeshComponent> orbitLocationGizmoMesh = editorController->CreateComponent<StaticMeshComponent>("GizmoMesh");
    orbitLocationGizmoMesh->SetMaterial("assets:\\materials\\wireframe.xml");
    orbitLocationGizmoMesh->SetMesh("engine:\\mesh\\wireframe-sphere");
    orbitLocationGizmoMesh->scale = float3(0.2f);

	lightObject = CreateObject<GameObject>(ObjectId::Create("MainLightSource"));
	lightObject->transform->position = float3(0.0f, 6.0f, 0.0f);

	Ref<LightComponent> light = lightObject->CreateComponent<LightComponent>("Light");
    lightObject->SetTickEnabled(true);

    gridObject = CreateObject<GameObject>(ObjectId::Create("Grid"));

    Ref<StaticMeshComponent> gridMesh = gridObject->CreateComponent<StaticMeshComponent>("StaticMesh");
    gridMesh->SetMaterial("assets:\\materials\\dev-material.xml");
    gridMesh->SetMesh("assets:\\models\\plane.obj");
    gridMesh->scale = float3(4.0f, 1.0f, 4.0f);

    //RTTI::SaveObjectToBinary(gridObject, "assets:\\test.asset");

    // create an initial particle system
    AddNewDefaultEffect("particle-system", true);

    //activeParticleSystem = Deserialise::ObjectFromXML("assets:\\entity\\test.xml");

    //editableParticleSystems.push_back(activeParticleSystem);

    //std::unordered_map<ComponentId, Ref<Component>> components;

    //Serialise::RTTIObjectToXML(activeParticleSystem, "assets:\\entity\\test.xml");
    //Ref<Object> loaded = Deserialise::ObjectFromXML("assets:\\entity\\test.xml");
}

//////////////////////////////////////////////////////////////////////////

void ParticleEditorApplication::OnPostUpdate()
{
    Application::OnPostUpdate();

    const f32 dt = Time::GetDeltaTime();
    audioBeepTimer += dt;

    if (audioBeepTimer >= 1.0f)
    {
        audioBeepTimer = 0.0f;
        //Audio::PlayAudio("assets:\\audio\\8BitSoundPack\\Money.wav");
    }
}

//////////////////////////////////////////////////////////////////////////

void ParticleEditorApplication::OnImGuiRender()
{
    Application::OnImGuiRender();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 2.0f);
    ImGui::SetNextWindowPos(ImVec2(4, 8));
    if (ImGui::Begin("Objects", (bool*)0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse))
    {
        if (lightObject != nullptr)
        {
            RTTI::DisplayObjectProperties("Light", lightObject.get());
            ImGui::Separator();
        }

        if (activeParticleSystem)
        {
            RTTI::DisplayEdit<Ref<RTTIObject>>("ParticleSystem", std::dynamic_pointer_cast<RTTIObject>(activeParticleSystem));
        }

        ImGui::End();
    }

    ImGui::PopStyleVar();
}

//////////////////////////////////////////////////////////////////////////

void ParticleEditorApplication::AddNewDefaultEffect(const std::string& id, bool makeActive/* = false*/, const float3& atLocation/* = float3(0.0f)*/)
{
    Ref<Object> effect = CreateDefaultParticleEffect(id, editableParticleSystems.size());// Deserialise::ObjectFromXML("assets:\\entity\\test.xml");

    Ref<TransformComponent> transform = effect->CreateComponent<TransformComponent>("RootTransform");
    transform->position = atLocation;

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
