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
#include "Core/Utility/FileVolumeManager.h"

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

void SandboxApp::OnPreUpdate(const f32 dt)
{
    Application::OnPreUpdate(dt);

    // jsut for fun. I should put this into like a component or some shit.
    if (InputManager::GetInputState(ImpulseBurstAction) == KeyboardState::Pressed)
    {
        Physics::ApplyImpulseInRadius(player->transform->position, 4.0f, 500.0f, { player.get() });
    }
}

//////////////////////////////////////////////////////////////////////////

void SandboxApp::OnRenderCreate()
{
    Application::OnRenderCreate();

    window->Recentre();
}

//////////////////////////////////////////////////////////////////////////

void SandboxApp::OnPostCreate()
{
    // do our game initialisation here
    // this is called from the main thread

    physics = CreateObject<PhysicsWorld>(ObjectId::Create("DiscretePhysicsWorld"));

    LevelLoader::LoadFromFile("assets/maps/test_arena.xml", level);

    player = CreateObject<Player>(ObjectId::Create("Player"));
    player->SetPositionAndRotation(level.spawnpoint, float3(0.0f));

    particleObject = CreateObject<GameObject>(ObjectId::Create("Test Particle Effect"));
    Ref<ParticleSystemComponent> particleEmitter = particleObject->CreateComponent<ParticleSystemComponent>("ParticleSystem");
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
    InputManager::RegisterInput(ImpulseBurstAction, { KeyboardKey::R });

    PathResult boxAssetpath = FileVolumeManager::GetRealPathFromVirtualPath("assets:\\models\\box.obj");
    PathResult cylinderAssetpath = FileVolumeManager::GetRealPathFromVirtualPath("assets:\\models\\cylinder.obj");

    if (boxAssetpath.valid)
    {
        printf("found %s.\n", boxAssetpath.fullpath.c_str());
    }

    if (cylinderAssetpath.valid)
    {
        printf("found %s.\n", cylinderAssetpath.fullpath.c_str());
    }
}

//////////////////////////////////////////////////////////////////////////

void SandboxApp::OnDestroy()
{
    // destroy anything that is relevent
    // this is called on the main thread

    DestroyObject(player->GetId());
    DestroyObject(physics->GetId());
}

//////////////////////////////////////////////////////////////////////////
