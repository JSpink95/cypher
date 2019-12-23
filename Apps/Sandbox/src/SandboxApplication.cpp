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
#include "Particle/ParticleEmitter.h"

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

Ref<PhysicsObject> CreatePhysicsBoxFromPosSize(const f32 mass, const float3& position, const float3& hs)
{
    Ref<PhysicsObject> object = CreateObject<PhysicsObject>();

    Ref<BoxCollision> boxCollision = std::make_shared<BoxCollision>(hs);
    object->Initialise(mass, position, boxCollision);
    
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
    object->Initialise(mass, position, sphereCollision);

    Ref<StaticMeshComponent> sphereMesh = object->NewComponent<StaticMeshComponent>();
    sphereMesh->SetMaterial(MaterialLibrary::GetMaterial("mesh-lit-tex-checkerboard"));
    sphereMesh->SetMesh(MeshLibrary::GetMesh("game:mesh-sphere-ss"));
    sphereMesh->SetScale(float3(radius));

    return object;
}

//////////////////////////////////////////////////////////////////////////

void LoadMapFromFile(const std::string& filepath, std::vector<Ref<PhysicsObject>>& objects)
{
    std::ifstream file(filepath.c_str());
    if (file.fail())
    {
        return;
    }

    //
    // syntax
    // box <min>x,y,z,<max>x,y,z,<mass>
    // sphere <position>x,y,z,<radius>,<mass>

    std::string line;
    while (std::getline(file, line))
    {
        if (line.length() == 0 || line.at(0) == '#')
            continue;

        size_t typeEnd = line.find_first_of(' ');
        std::string type = line.substr(0, typeEnd);
        std::string input = line.substr(typeEnd + 1);

        auto readNextInput = [](std::string& input) -> std::string
        {
            size_t next = input.find_first_of(',');
            if (next == std::string::npos)
                return input;

            std::string value = input.substr(0u, next);
            input = input.substr(next + 1);
            return value;
        };

        auto isValidFloatDigit = [](const char c) -> bool
        {
            return ((c >= '0' && c <= '9') || (c == '.' || c == '-'));
        };

        auto readFloatList = [isValidFloatDigit](const std::string& input, std::vector<f32>& floats) -> void
        {
            std::string floatValue;
            for (char c : input)
            {
                if (isValidFloatDigit(c))
                {
                    floatValue += c;
                }
                else if (c == ',' || c == ';')
                {
                    floats.push_back(std::stof(floatValue));
                    floatValue = "";
                }
            }
        };

        if (type == "box")
        {
            printf("Creating a box...");

            std::vector<f32> floats;
            readFloatList(input, floats);

            float3 min = float3(-1.0f);
            float3 max = float3(1.0f);
            f32 mass = 0.0f;

            if (floats.size() >= 3)
            {
                min = { floats.at(0), floats.at(1), floats.at(2) };
            }

            printf("min(%.3f,%.3f,%.3f),", min.x, min.y, min.z);

            if (floats.size() >= 6)
            {
                max = { floats.at(3), floats.at(4), floats.at(5) };
            }

            printf("max(%.3f,%.3f,%.3f),", max.x, max.y, max.z);

            if (floats.size() >= 7)
            {
                mass = floats.at(6);
            }

            printf("mass(%.3f)...finished!\n", mass);

            objects.push_back(CreatePhysicsBoxFromMinMax(mass, min, max));
        }
        else if (type == "sphere")
        {
            printf("Creating a sphere...");

            std::vector<f32> floats;
            readFloatList(input, floats);

            float3 position = float3(0.0f);
            f32 radius = 1.0f;
            f32 mass = 0.0f;

            if (floats.size() >= 3)
            {
                position = { floats.at(0), floats.at(1), floats.at(2) };
            }

            printf("position(%.3f),", position.x, position.y, position.z);

            if (floats.size() >= 4)
            {
                radius = floats.at(3);
            }

            printf("radius(%.3f),", radius);

            if (floats.size() >= 5)
            {
                mass = floats.at(4);
            }

            printf("mass(%.3f)...finished!\n", mass);

            objects.push_back(CreatePhysicsSphereFromPosRad(mass, position, radius));
        }
        else
        {
            printf("unsupported type %s.\n", type.c_str());
        }
    }

    file.close();
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

void SandboxApp::OnCreate()
{
    // do our game initialisation here
    // this is called from the main thread

    physics = CreateObject<PhysicsWorld>("DiscretePhysicsWorld");

    //cameraController = CreateObject<FpsCameraController>();
    player = CreateObject<Player>();
	Ref<LightComponent> light = player->NewComponent<LightComponent>();
	light->GetLightInstance()->SetColor(Color::WarmWhite);

    weapon = CreateObject<Weapon>();
    weapon->SetOwner(player.get());

    GetGameThread()->PushThreadTask(this, &SandboxApp::LoadRenderResources);

    physics->SetSimulatePhysics(true);

    InputManager::RegisterInput(ImpulseBurstAction, { KeyboardKey::R });
}

//////////////////////////////////////////////////////////////////////////

void SandboxApp::OnDestroy()
{
    // destroy anything that is relevent
    // this is called on the main thread

    for (Ref<PhysicsObject> po : physicsObjects)
    {
        DestroyObject(po->GetInstanceId());
    }

    physicsObjects.clear();

    DestroyObject(player->GetInstanceId());
    DestroyObject(weapon->GetInstanceId());
    DestroyObject(physics->GetInstanceId());
}

//////////////////////////////////////////////////////////////////////////

void SandboxApp::LoadRenderResources()
{
    //GetWindowContext()->SetWindowPosition(int2(-1920 + 200, 200));

    LoadMapFromFile("assets/maps/test_map.map", physicsObjects);
    for (Ref<PhysicsObject> po : physicsObjects)
    {
        RenderPassManager::AddObjectToPass(RenderPassType::Opaque, po);
        RenderPassManager::AddObjectToPass(RenderPassType::Shadow, po);
    }

    hasLoadedRenderResources = true;
}

//////////////////////////////////////////////////////////////////////////
