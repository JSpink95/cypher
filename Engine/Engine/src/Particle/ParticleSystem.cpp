//////////////////////////////////////////////////////////////////////////
//    File        	: ParticleSystem.cpp
//    Created By    : Jack Spink
//    Created On 	: [29/10/2019]
//////////////////////////////////////////////////////////////////////////

#ifdef _OPENGL
    #include "glfw.h"
#endif

//////////////////////////////////////////////////////////////////////////

#include "Particle/ParticleSystem.h"
#include "Particle/ParticleEmitter.h"
#include "Particle/ParticleUpdater.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Core.h"
#include "Core/Object.h"
#include "Core/Thread/GameThread.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass.h"
#include "Render/Platform/ApiManager.h"
#include "Render/Platform/VertexArray.h"
#include "Render/Platform/Buffer.h"
#include "Render/Platform/Renderer.h"
#include "Render/Utility/TextureLibrary.h"
#include "Render/Utility/MaterialLibrary.h"

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Component/TransformComponent.h"
#include "GameFramework/Camera/Camera.h"

//////////////////////////////////////////////////////////////////////////

#include <algorithm>

//////////////////////////////////////////////////////////////////////////

struct UniformBufferParticleExtra
{
    vec4 cameraPosition;
};

//////////////////////////////////////////////////////////////////////////

void ParticleManager::Update(f32 dt)
{
    auto isParticleDead = [](Particle const& particle) -> bool { return particle.age <= 0.0f; };
    particles.erase(std::remove_if(particles.begin(), particles.end(), isParticleDead), particles.end());

    for (Particle& particle : particles)
    {
        particle.data.position += particle.velocity * dt;
        particle.age -= glm::min(dt, particle.age);
    }
}

//////////////////////////////////////////////////////////////////////////

void ParticleManager::BuildParticleBatch(std::vector<ParticleVertex>& vertices)
{
    for (Particle& particle : particles)
        vertices.push_back(particle.data);
}

//////////////////////////////////////////////////////////////////////////

void ParticleSystemComponent::OnConstruct()
{
    Super::OnConstruct();

    GetGameThread()->PushThreadTask(this, &ParticleSystemComponent::RenderTask_InitialiseParticleMesh);

    if (owner != nullptr)
    {
        ownerTransform = owner->FindFirstComponentOfType<TransformComponent>();
    }

    particleMaterial = MaterialLibrary::GetMaterial("particle-default");
}

//////////////////////////////////////////////////////////////////////////

void ParticleSystemComponent::OnUpdate(const f32 dt)
{
    Super::OnUpdate(dt);

    if (emitter != nullptr)
    {
        emitter->Tick(dt, this);
    }

    if (updater != nullptr)
    {
        for (Particle& particle : manager) updater->Update(dt, particle);
    }

    manager.Update(dt);
}

//////////////////////////////////////////////////////////////////////////

void ParticleSystemComponent::OnRender(RenderPassType::Enum pass, Ref<Material> materialOverride)
{
    Super::OnRender(pass, materialOverride);

    if (pass != RenderPassType::Particle)
    {
        // we only render particles in the... particle pass.
        return;
    }

    mat4 const worldTransform = localSpaceParticles ? CalculateTransformMatrix() : fmat4(1.0f);
    if (particleMaterial != nullptr && particleBuffer != nullptr)
    {
        std::vector<ParticleVertex> particles;
        manager.BuildParticleBatch(particles);

        particleBuffer->UpdateBuffer(particles.size(), sizeof(ParticleVertex), (particles.size() == 0) ? nullptr : &particles.at(0));

        UniformBufferParticleExtra particleExtra;
        particleExtra.cameraPosition = vec4(Camera::GetActiveCamera()->GetEyePosition(), 1.0f);

        particleMaterial->SetParameterBlock<UniformBufferParticleExtra>("ParticleExtra", particleExtra);
        particleMaterial->SetParameterValue<MaterialParameterTexture2D>("uTexture", TextureLibrary::GetTexture(EngineTextureId::SphereWithAlpha));

        // #todo - replace hack with proper render mode setting
        GlCall(Renderer::SetRenderMode(GL_POINTS));
        Renderer::Submit(particleMaterial, particleMesh, worldTransform);
        GlCall(Renderer::SetRenderMode(GL_TRIANGLES));
    }
}

//////////////////////////////////////////////////////////////////////////

fmat4 ParticleSystemComponent::CalculateTransformMatrix() const
{
    mat4 worldTransform = fmat4(1.0f);

    if (!ownerTransform.expired())
    {
        // take our parent object transform
        worldTransform *= ownerTransform.lock()->CalculateTransformMatrix();
    }

    // append our actual transform
    worldTransform *= Super::CalculateTransformMatrix();

    return worldTransform;
}

//////////////////////////////////////////////////////////////////////////

void ParticleSystemComponent::RenderTask_InitialiseParticleMesh()
{
    static VertexBufferLayout particleLayout = {
        { "aPosition", ShaderData::Float3 },
        { "aSize", ShaderData::Float2 },
    };

    particleBuffer = GetApiManager()->CreateVertexBuffer(0, sizeof(ParticleVertex), nullptr);
    particleBuffer->SetLayout(particleLayout);

    particleMesh = GetApiManager()->CreateVertexArray();
    particleMesh->AddBuffer(particleBuffer);
}

//////////////////////////////////////////////////////////////////////////

void ParticleSystemComponent::PushParticle(const float3& position, const float2& size, const float3& velocity, const f32 age)
{
    manager.particles.push_back({ { position, size }, velocity, age });
}

//////////////////////////////////////////////////////////////////////////
