//////////////////////////////////////////////////////////////////////////
//    File        	: SandboxApplication.cpp
//    Created By    : Jack Spink
//    Created On 	: [24/9/2019]
//////////////////////////////////////////////////////////////////////////

#include "SandboxApplication.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/RTTI.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Object.h"
#include "Core/ObjectManager.h"
#include "Core/Component.h"
#include "Core/ComponentRef.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Utility/RandomUtils.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Mesh.h"
#include "Render/Platform/Window.h"
#include "Render/Platform/Buffer.h"
#include "Render/Platform/Material.h"
#include "Render/Platform/VertexArray.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Utility/MaterialLibrary.h"

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Camera/PerspectiveCamera.h"
#include "GameFramework/Component/TransformComponent.h"
#include "GameFramework/Component/StaticMeshComponent.h"
#include "GameFramework/Component/PerspectiveCameraComponent.h"
#include "GameFramework/Component/LightComponent.h"

//////////////////////////////////////////////////////////////////////////

class PlayerControllerComponent : public Component
{
    DECLARE_COMPONENT(PlayerControllerComponent, Component)
public:

    PlayerControllerComponent()
    {
        tickFunction.enabled = true;
    }

    virtual void OnConstruct() override
    {
        Super::OnConstruct();

        transform.componentName = "RootTransform";
        transform.OnConstruct(owner);
    }

    virtual void OnTick(const f32 dt) override
    {
        Super::OnTick(dt);

        time += dt;

        if (transform)
        {
            const f32 angle = glm::radians(time * 45.0f);
            const f32 dist = 5.0f + (glm::sin(glm::radians(time * 100.0f)) * 0.5f + 0.5f) * 5.0f;
            transform->position = float3(glm::cos(angle) * 6.0f, dist, glm::sin(angle) * 6.0f);
        }
    }

private:
    f32 time = 0.0f;
    ComponentRef<TransformComponent> transform;
};

//////////////////////////////////////////////////////////////////////////

struct LevelVertex
{
    static inline VertexBufferLayout layout = {
        { "aPosition", ShaderData::Float3 },
        { "aNormal", ShaderData::Float3 },
        { "aTexcoord", ShaderData::Float2 },
        { "aTextureId", ShaderData::Float },
    };

    float3 position;
    float3 normal;
    float2 texcoord;
    f32 textureId;
};

//////////////////////////////////////////////////////////////////////////

template<size_t width, size_t height>
class Level
{
public:
    Level(s32 seed);

    inline constexpr const size_t GetIndexFromPos(const size_t x, const size_t y) const
    {
        return x + y * width;
    }

    inline constexpr const bool GetActiveState(const size_t x, const size_t y) const
    {
        if (x < 0u || y < 0u || x >= width || y >= height) return false;
        return cells[GetIndexFromPos(x, y)];
    }

public:
    Ref<Mesh> GenerateMesh();

public:
    bool cells[width * height];
};

//////////////////////////////////////////////////////////////////////////

template<size_t width, size_t height>
Level<width, height>::Level(s32 seed)
{
    std::default_random_engine engine(seed);

    for (size_t x = 0u; x < width; ++x)
    {
        for (size_t y = 0; y < height; ++y)
        {
            const size_t index = x + y * width;
            const f32 a = Random::Float();

            cells[index] = (a > 60.0f);
        }
    }
}

//////////////////////////////////////////////////////////////////////////

template<size_t width, size_t height>
Ref<Mesh> Level<width, height>::GenerateMesh()
{
    std::vector<LevelVertex> vertices;

    for (size_t x = 0u; x < width; ++x)
    {
        for (size_t y = 0; y < height; ++y)
        {
            float3 position = float3((f32)x, 0.0f, (f32)y);
            float2 texcoord = float2(position.x, position.z);
        }
    }

    return nullptr;
}

//////////////////////////////////////////////////////////////////////////

void SandboxApplication::OnPostCreate()
{
    Application::OnPostCreate();

    window->Recentre();
    window->SetWindowPosition(int2(-1920, 200));

    camera = CreateObject<Object>(ObjectId::Create("camera"));

    Ref<TransformComponent> transform = camera->CreateComponent<TransformComponent>("RootTransform");
    transform->position = float3(0.0f, 3.0f, 2.0f);

    Ref<PlayerControllerComponent> controller = camera->CreateComponent<PlayerControllerComponent>("Controller");

    Ref<PerspectiveCameraComponent> cam = camera->CreateComponent<PerspectiveCameraComponent>("cam");
    cam->SetTickEnabled(true);
    cam->SetAsMainCamera();
    cam->SetTarget(float3(0.0f));

    scene = CreateObject<Object>(ObjectId::Create("scene"));

    Ref<LightComponent> light = scene->CreateComponent<LightComponent>("Light");
    light->SetTickEnabled(true);
    light->color = float3(1.0f * 1.0f);
    light->position = float3(0.0f, 2.0f, 0.0f);

    for (s32 x = -4; x <= 4; ++x)
    {
        for (s32 z = -4; z <= 4; ++z)
        {
            Ref<StaticMeshComponent> mesh = scene->CreateComponent<StaticMeshComponent>(std::to_string(x) + ":" + std::to_string(z));
            mesh->SetMaterial(MaterialLibrary::GetMaterial("assets:\\materials\\dev-material.xml"));
            mesh->SetMesh("assets:\\models\\box.obj");
            mesh->position = float3((f32)x, Random::Float(-0.5f, 0.5f), (f32)z);
            mesh->scale = float3(0.5f, 0.5f, 0.5f);
        }
    }

    Ref<StaticMeshComponent> mesh = scene->CreateComponent<StaticMeshComponent>("emissive");

    struct EmissiveData
    {
        float3 color;
        f32 strength;
    };

    EmissiveData data = { float3(1.0f, 0.3f, 0.1f), 5.0f };

    Ref<Material> emissiveMat = MaterialLibrary::GetMaterial("assets:\\materials\\mesh-emissive.xml")->Clone();
    emissiveMat->SetParameterBlock("EmissiveData", data);

    mesh->SetMaterial(emissiveMat);
    mesh->SetMesh("assets:\\models\\capsule.obj");
    mesh->position = float3(0.0f, 2.0f, 0.0f);
    mesh->scale = float3(0.05f, 0.4f, 0.05f);

}

//////////////////////////////////////////////////////////////////////////

void SandboxApplication::OnDestroy()
{
    Application::OnDestroy();
}

//////////////////////////////////////////////////////////////////////////

void main()
{
    Application::Main<SandboxApplication>();
}

//////////////////////////////////////////////////////////////////////////
