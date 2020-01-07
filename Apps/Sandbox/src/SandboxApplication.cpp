//////////////////////////////////////////////////////////////////////////
//    File        	: SandboxApplication.cpp
//    Created By    : Jack Spink
//    Created On 	: [24/9/2019]
//////////////////////////////////////////////////////////////////////////

#include "glfw.h"

//////////////////////////////////////////////////////////////////////////

#include "SandboxApplication.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Core.h"
#include "Core/Object.h"
#include "Core/ObjectManager.h"
#include "Core/Thread/GameThread.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/StaticMeshObject.h"
#include "Render/Platform/ApiManager.h"
#include "Render/Platform/Window.h"
#include "Render/Platform/Renderer.h"
#include "Render/Platform/ComputeShader.h"
#include "Render/Platform/VertexArray.h"
#include "Render/Platform/Buffer.h"
#include "Render/Platform/Framebuffer.h"
#include "Render/Platform/Material.h"
#include "Render/Platform/MaterialParameter.h"
#include "Render/Platform/Shader.h"
#include "Render/Platform/Texture2D.h"
#include "Render/Platform/RenderPass.h"
#include "Render/Platform/CoreMaterialParameterBlocks.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Utility/MeshLibrary.h"
#include "Render/Utility/MaterialLibrary.h"
#include "Render/Utility/ObjMeshLoader.h"
#include "Render/Utility/TextureLibrary.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Effects/LightManager.h"

//////////////////////////////////////////////////////////////////////////

#include "Physics/PhysicsWorld.h"
#include "Physics/PhysicsObject.h"

//////////////////////////////////////////////////////////////////////////

#include "Particle/ParticleUpdater.h"
#include "Particle/ParticleSystem.h"

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Camera/Camera.h"
#include "GameFramework/Camera/PerspectiveCamera.h"
#include "GameFramework/Component/LightComponent.h"
#include "GameFramework/Component/TransformComponent.h"
#include "GameFramework/Component/StaticMeshComponent.h"

//////////////////////////////////////////////////////////////////////////

#include "Physics/Geometry/BoxCollisionShape.h"
#include "Physics/Geometry/PlaneCollisionShape.h"
#include "Physics/Geometry/SphereCollisionShape.h"

//////////////////////////////////////////////////////////////////////////

#include "Dungeon/DungeonRoom.h"
#include "Dungeon/FpsCameraController.h"
#include "Dungeon/Weapon.h"

//////////////////////////////////////////////////////////////////////////

#include <random>
#include <fstream>

//////////////////////////////////////////////////////////////////////////

#include "Player.h"

//////////////////////////////////////////////////////////////////////////

static const std::string ToggleMouseAction = "ToggleMouse";
static const std::string ImpulseBurstAction = "ImpulseBurst";

//////////////////////////////////////////////////////////////////////////

struct Color
{
    static inline vec4 White = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    static inline vec4 Red = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    static inline vec4 Green = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    static inline vec4 Blue = vec4(0.0f, 0.0f, 1.0f, 1.0f);

    static inline vec4 WarmWhite = vec4(1.0f, 0.99f, 0.8f, 1.0f);
    static inline vec4 WarmRed = vec4(1.0f, 0.2f, 0.2f, 1.0f);
    static inline vec4 WarmGreen = vec4(0.2f, 1.0f, 0.2f, 1.0f);
    static inline vec4 WarmBlue = vec4(0.2f, 0.2f, 1.0f, 1.0f);
};

//////////////////////////////////////////////////////////////////////////

class ParticleEventOnDeath : public ParticleEvent
{
public:
    virtual void OnEvent(Particle& particle) override
    {
        // spawn shit, another emitter?

    };
};

//////////////////////////////////////////////////////////////////////////

Ref<PhysicsObject> CreatePhysicsBoxFromPosSize(const f32 mass, const float3& position, const float3& hs)
{
    Ref<PhysicsObject> object = CreateObject<PhysicsObject>();

    Ref<BoxCollision> boxCollision = std::make_shared<BoxCollision>(hs);
    object->Initialise(mass, position, float3(0.0f), boxCollision);
    
    Ref<StaticMeshComponent> boxMesh = object->NewComponent<StaticMeshComponent>();
    boxMesh->SetMaterial(MaterialLibrary::GetMaterial("mesh-lit-tex-checkerboard"));
    boxMesh->SetMesh(MeshLibrary::GetMesh("game:mesh-box"));
    boxMesh->SetScale(hs);

    return object;
}

//////////////////////////////////////////////////////////////////////////

Ref<PhysicsObject> CreatePhysicsBoxFromMinMax(const f32 mass, const float3& min, const float3& max)
{
    const float3 pos = (min + max) / 2.0f;
    const float3 hs = (max - min) / 2.0f;

    return CreatePhysicsBoxFromPosSize(mass, pos, hs);
}

//////////////////////////////////////////////////////////////////////////

Ref<PhysicsObject> CreatePhysicsSphereFromPosRad(const f32 mass, const float3& position, const f32 radius)
{
    Ref<PhysicsObject> object = CreateObject<PhysicsObject>();

    Ref<SphereCollision> sphereCollision = std::make_shared<SphereCollision>(radius);
    object->Initialise(mass, position, float3(0.0f), sphereCollision);

    Ref<StaticMeshComponent> sphereMesh = object->NewComponent<StaticMeshComponent>();
    sphereMesh->SetMaterial(MaterialLibrary::GetMaterial("mesh-lit-tex-checkerboard"));
    sphereMesh->SetMesh(MeshLibrary::GetMesh("game:mesh-sphere-ss"));
    sphereMesh->SetScale(float3(radius));

    return object;
}

//////////////////////////////////////////////////////////////////////////

void SandboxApp::OnPreRender()
{
    Application::OnPreRender();

    static bool inputEnabled = true;
    if (InputManager::GetInputState(ToggleMouseAction) == KeyboardState::Pressed)
    {
        inputEnabled = !inputEnabled;
    }

    if (InputManager::GetInputState(ImpulseBurstAction) == KeyboardState::Pressed)
    {
        Physics::ApplyImpulseInRadius(player->transform->position, 4.0f, 500.0f, { player.get() });
    }

    RenderPassManager::Render();
}

//////////////////////////////////////////////////////////////////////////

void SandboxApp::OnPostCreate()
{
    // do our game initialisation here
    // this is called from the main thread

    physics = CreateObject<PhysicsWorld>("DiscretePhysicsWorld");

    LevelLoader::LoadFromFile("assets/maps/test_arena.xml", level);

    player = CreateObject<Player>();
    player->SetPositionAndRotation(level.spawnpoint, float3(0.0f));

    particleObject = CreateObject<GameObject>();
    Ref<ParticleSystemComponent> particleEmitter = particleObject->NewComponent<ParticleSystemComponent>();
    particleEmitter->SetEmissionRate(0.1f);
    particleEmitter->SetMaxParticlesAlive(1024);

    Ref<ParticlePointEmitter> pointEmitter = std::make_shared<ParticlePointEmitter>();
    pointEmitter->SetPoint(float3(0.0f));

    Ref<ParticleSetLifetimeRandom> randomLifetime = std::make_shared<ParticleSetLifetimeRandom>();
    randomLifetime->SetRange(2.0f, 4.0f);

    Ref<ParticleSetVelocityRandom> randomVelocity = std::make_shared<ParticleSetVelocityRandom>();
    randomVelocity->SetStrength(float2(6.0f, 20.0f));
    randomVelocity->SetMinDirection(float3(-0.3f, 1.0f, -0.3f));
    randomVelocity->SetMaxDirection(float3( 0.3f, 1.0f,  0.3f));

    Ref<ParticleSetSizeRandom> randomSize = std::make_shared<ParticleSetSizeRandom>();
    randomSize->SetMinSize(0.6f);
    randomSize->SetMaxSize(0.8f);

    Ref<ParticleGravity> gravity = std::make_shared<ParticleGravity>();
    gravity->SetGravity(float3(0.0f, -1.0f, 0.0f));

    Ref<ParticleLinearDrag> linearDrag = std::make_shared<ParticleLinearDrag>();
    linearDrag->SetDrag(4.0f);

    Ref<ParticleEventOnDeath> onParticleDeath = std::make_shared<ParticleEventOnDeath>();

    particleEmitter->GetEmissionStage()->AddOutput(pointEmitter);
    particleEmitter->GetEmissionStage()->AddOutput(randomLifetime);
    particleEmitter->GetEmissionStage()->AddOutput(randomVelocity);
    particleEmitter->GetEmissionStage()->AddOutput(randomSize);

    particleEmitter->GetUpdateStage()->AddOutput(gravity);
    particleEmitter->GetUpdateStage()->AddOutput(linearDrag);

    particleEmitter->SetParticleDeathEvent(onParticleDeath);

    {
        GetGameThread()->AddObject(particleObject);
        RenderPassManager::AddObjectToPass(RenderPassType::Particle, particleObject);
    }

    physics->SetSimulatePhysics(true);

    GetWindowContext()->SetWindowPosition(int2(-1920 + 200, 200));

    //GetGameThread()->PushThreadTask(this, &SandboxApp::LoadRenderResources);

    InputManager::RegisterInput(ImpulseBurstAction, { KeyboardKey::R });
}

//////////////////////////////////////////////////////////////////////////

void SandboxApp::OnDestroy()
{
    // destroy anything that is relevent
    // this is called on the main thread

    DestroyObject(player->GetInstanceId());
    DestroyObject(physics->GetInstanceId());
}

//////////////////////////////////////////////////////////////////////////

void SandboxApp::LoadRenderResources()
{
    hasLoadedRenderResources = true;
}

//////////////////////////////////////////////////////////////////////////
