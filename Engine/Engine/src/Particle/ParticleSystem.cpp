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
#include "Particle/ParticleUpdater.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Core.h"
#include "Core/Object.h"
#include "Core/RTTI/RTTI.h"
#include "Core/Thread/GameThread.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass.h"
#include "Render/Platform/ApiManager.h"
#include "Render/Platform/VertexArray.h"
#include "Render/Platform/Renderer.h"
#include "Render/Utility/TextureLibrary.h"
#include "Render/Utility/MaterialLibrary.h"

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Component/TransformComponent.h"
#include "GameFramework/Camera/Camera.h"

//////////////////////////////////////////////////////////////////////////

#include <algorithm>

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(ParticleSystemComponent, TransformComponent)
    RTTI_PROPERTY(ParticleSystemComponent, Ref<Material>, material)
    RTTI_PROPERTY(ParticleSystemComponent, bool, localSpaceParticles)
    RTTI_PROPERTY(ParticleSystemComponent, f32, emissionRate)
    RTTI_PROPERTY(ParticleSystemComponent, s32, maxAliveParticles)
    RTTI_PROPERTY(ParticleSystemComponent, Ref<ParticleEmissionStage>, emissionStage)
    RTTI_PROPERTY(ParticleSystemComponent, Ref<ParticleUpdateStage>, updateStage)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

struct UniformBufferParticleExtra
{
    vec4 cameraPosition;
};

//////////////////////////////////////////////////////////////////////////

ParticleSystemComponent::ParticleSystemComponent()
{
    parentTransform.componentName = "RootTransform";
}

//////////////////////////////////////////////////////////////////////////

void ParticleSystemComponent::OnConstruct()
{
    Super::OnConstruct();

    GetGameThread()->PushThreadTask(this, &ParticleSystemComponent::RenderTask_InitialiseParticleMesh);

    updateStage = std::make_shared<ParticleUpdateStage>();
    emissionStage = std::make_shared<ParticleEmissionStage>();
    material = MaterialLibrary::GetMaterial("assets:\\materials\\particle-default.xml");    // load the default material
}

//////////////////////////////////////////////////////////////////////////

void ParticleSystemComponent::OnUpdate(const f32 dt)
{
    Super::OnUpdate(dt);

    timeSinceLastEmission += dt;
    if (timeSinceLastEmission >= emissionRate)
    {
        ++particleCountRequest;
        timeSinceLastEmission = 0.0f;
    }

    s32 maxAllowedSpawns = glm::min(maxAliveParticles - (s32)particles.size(), particleCountRequest);

    mat4 transform = CalculateTransformMatrix();
    for (s32 i = 0; i < maxAllowedSpawns; ++i)
    {
        Particle particle;
        emissionStage->Initialise(particle);

        // if we are not local space particles (our position doesn't follow the particle effect if it moves),
        // then we emit our particle at the currently located position.

        if (!localSpaceParticles)
        {
            particle.data.position = float3(transform * float4(particle.data.position, 1.0f));
        }

        particles.push_back(particle);
        numParticlesSpawned += 1;
    }

    particleCountRequest = glm::max(0, particleCountRequest - maxAllowedSpawns);
    for (Particle& particle : particles)
    {
        updateStage->Update(dt, particle);
        particle.data.position += particle.velocity * dt;
        particle.age -= dt;

        if (onParticleDeath && particle.age <= 0.0f)
        {
            onParticleDeath->OnEvent(particle);
        }
    }

    auto isParticleDead = [](const Particle& particle) -> bool { return particle.age <= 0.0f; };
    particles.erase(std::remove_if(particles.begin(), particles.end(), isParticleDead), particles.end());
}

//////////////////////////////////////////////////////////////////////////

void ParticleSystemComponent::OnRender(RenderPassType::Enum pass, Ref<Material> materialOverride)
{
    Super::OnRender(pass, materialOverride);

    if (pass != RenderPassType::Particle)
    {
        // we only render particles in the particle pass.
        return;
    }

    mat4 const worldTransform = localSpaceParticles ? CalculateTransformMatrix() : fmat4(1.0f);
    if (material != nullptr && particleBuffer != nullptr)
    {
        std::vector<ParticleVertex> vertices;
        std::transform(particles.begin(), particles.end(), std::back_inserter(vertices), [](Particle& particle) -> ParticleVertex { return particle.data; });

        particleBuffer->UpdateBuffer(vertices.size(), sizeof(ParticleVertex), (vertices.size() == 0) ? nullptr : &vertices.at(0));

        UniformBufferParticleExtra particleExtra;
        particleExtra.cameraPosition = vec4(CameraBase::GetActiveCamera()->GetEyeLocation(), 1.0f);

        material->SetParameterBlock<UniformBufferParticleExtra>("ParticleExtra", particleExtra);

        Renderer::Submit(material, particleMesh, worldTransform);
    }
}

//////////////////////////////////////////////////////////////////////////

void ParticleSystemComponent::RequestNewParticles(s32 count/* = 1*/)
{
    particleCountRequest += count;
}

//////////////////////////////////////////////////////////////////////////

void ParticleSystemComponent::RenderTask_InitialiseParticleMesh()
{
    particleBuffer = GetApiManager()->CreateVertexBuffer(0, sizeof(ParticleVertex), nullptr);
    particleBuffer->SetLayout(ParticleVertex::layout);

    particleMesh = GetApiManager()->CreateVertexArray();
    particleMesh->AddBuffer(particleBuffer);
}

//////////////////////////////////////////////////////////////////////////
