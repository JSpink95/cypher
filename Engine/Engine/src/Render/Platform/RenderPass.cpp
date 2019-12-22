//////////////////////////////////////////////////////////////////////////
//    File        	: RenderPass.cpp
//    Created By    : Jack Spink
//    Created On 	: [23/10/2019]
//////////////////////////////////////////////////////////////////////////

#ifdef _OPENGL
#include "glfw.h"
#endif

#include "Render/Platform/RenderPass.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Core.h"
#include "Core/Object.h"
#include "Core/Application.h"
#include "Core/Thread/GameThread.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Window.h"
#include "Render/Platform/ApiManager.h"
#include "Render/Platform/Framebuffer.h"
#include "Render/Platform/Renderer.h"
#include "Render/Platform/Buffer.h"
#include "Render/Platform/VertexArray.h"
#include "Render/Platform/Texture2D.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Utility/MaterialLibrary.h"
#include "Render/Effects/LightManager.h"

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Camera/Camera.h"

//////////////////////////////////////////////////////////////////////////

#include <random>

//////////////////////////////////////////////////////////////////////////

static float2 const shadowFramebufferSize = float2(1024.0f);

//////////////////////////////////////////////////////////////////////////

void RenderPass::RegisterObject(Ref<Object> object)
{
    ObjectGuid id = object->GetInstanceId();
    if (objects.find(id) == objects.end())
    {
        objects.emplace(id, object);
    }
}

//////////////////////////////////////////////////////////////////////////

void RenderPass::DeregisterObject(Ref<Object> object)
{
    ObjectGuid id = object->GetInstanceId();
    objects.erase(id);
}

//////////////////////////////////////////////////////////////////////////

void RenderPass::Perform()
{
    for (auto keyval : objects)
    {
        ObjectGuid id = keyval.first;
        WeakRef<Object> object = keyval.second;

        if (object.expired())
            continue;

        object.lock()->OnRender(type, renderMaterialOverride);
    }
}

//////////////////////////////////////////////////////////////////////////

void RenderPassManager::Initialise()
{
    Ref<RenderPassManager> manager = GetRenderPassManager();
    if (manager)
    {
        manager->InitialiseImpl();
    }
}

//////////////////////////////////////////////////////////////////////////

void RenderPassManager::Render()
{
    Ref<RenderPassManager> manager = GetRenderPassManager();
    if (manager)
    {
        manager->RenderImpl();
    }
}

//////////////////////////////////////////////////////////////////////////

void RenderPassManager::AddObjectToPass(RenderPassType::Enum pass, Ref<Object> object)
{
    Ref<RenderPassManager> manager = GetRenderPassManager();
    if (manager)
    {
        manager->renderPasses[pass].RegisterObject(object);
    }
}

//////////////////////////////////////////////////////////////////////////

void RenderPassManager::RemoveObjectFromPass(RenderPassType::Enum pass, Ref<Object> object)
{
    Ref<RenderPassManager> manager = GetRenderPassManager();
    if (manager)
    {
        manager->renderPasses[pass].DeregisterObject(object);
    }
}

//////////////////////////////////////////////////////////////////////////

RenderPassManager::RenderPassManager()
    : renderPasses({
        RenderPass(RenderPassType::Opaque),
        RenderPass(RenderPassType::Transparent),
        RenderPass(RenderPassType::Shadow),
        RenderPass(RenderPassType::Particle)
      })
{

}

//////////////////////////////////////////////////////////////////////////

void RenderPassManager::InitialiseImpl()
{
	// setup window constants
	windowSize = (float2)GetApplication()->GetWindowContext()->GetWindowSize();
	framebufferSize = windowSize / 4.0f;

    // load materials
    sslMaterial = MaterialLibrary::GetMaterial("game:pp-ssl");
    ssaoMaterial = MaterialLibrary::GetMaterial("game:pp-ssao");
    imageBlurMaterial = MaterialLibrary::GetMaterial("game:pp-blur");
    imageBlendMaterial = MaterialLibrary::GetMaterial("game:pp-overlay");
    imageRenderMaterial = MaterialLibrary::GetMaterial("game:pp-image-render");

    // load framebuffers

    FramebufferData mainFbInit;                             // Enabled, FloatingPointPrecision
    mainFbInit.resolution = uint2(framebufferSize);
    mainFbInit.colorBuffers.at(GBuffer::CB_Albedo) = { true, false };
    mainFbInit.colorBuffers.at(GBuffer::CB_Position) = { true, true };
    mainFbInit.colorBuffers.at(GBuffer::CB_Normal) = { true, true };
    mainFbInit.colorBuffers.at(GBuffer::CB_DepthStencil) = { true, true };

    mainFramebuffer = GetApiManager()->CreateFramebuffer(mainFbInit);

    FramebufferData lightFbInit;
    lightFbInit.resolution = uint2(shadowFramebufferSize);
    lightFbInit.colorBuffers.at(GBuffer::CB_DepthStencil) = { true, true };

    lightFramebuffer = GetApiManager()->CreateFramebuffer(lightFbInit);

    FramebufferData sslFbInit;
    sslFbInit.resolution = uint2(framebufferSize);
    sslFbInit.colorBuffers.at(GBuffer::CB_Albedo) = { true, false };

    sslFramebuffer = GetApiManager()->CreateFramebuffer(sslFbInit);

    FramebufferData ssaoFbInit;
    ssaoFbInit.resolution = uint2(framebufferSize);
    ssaoFbInit.colorBuffers.at(GBuffer::CB_Albedo) = { true, false };

    ssaoFramebuffer = GetApiManager()->CreateFramebuffer(ssaoFbInit);

    FramebufferData particleFbInit;
    particleFbInit.resolution = uint2(framebufferSize);
    particleFbInit.colorBuffers.at(GBuffer::CB_Albedo) = { true, false };
    particleFbInit.colorBuffers.at(GBuffer::CB_DepthStencil) = { true, true };

    particleFramebuffer = GetApiManager()->CreateFramebuffer(particleFbInit);

    FramebufferData blurFbInit;
    blurFbInit.resolution = uint2(framebufferSize);
    blurFbInit.colorBuffers.at(GBuffer::CB_Albedo) = { true, false };

    blurFramebuffer = GetApiManager()->CreateFramebuffer(blurFbInit);

    FramebufferData imageBlendFbInit;
    imageBlendFbInit.resolution = uint2(framebufferSize);
    imageBlendFbInit.colorBuffers.at(GBuffer::CB_Albedo) = { true, false };

    blendFramebuffer = GetApiManager()->CreateFramebuffer(imageBlendFbInit);

    // setup ssao parameters
    {
        std::default_random_engine engine;
        std::uniform_real_distribution<f32> distributor(0.0f, 1.0f);

        float3 noisePixels[4 * 4];
        for (u32 index = 0; index < 4 * 4; ++index)
        {
            noisePixels[index] = float3(
                distributor(engine) * 2.0f - 1.0f,
                distributor(engine) * 2.0f - 1.0f,
                0.0f
            );
        }

        Ref<Texture2D> noiseTexture = GetApiManager()->CreateTexture2D(uint2(4, 4), noisePixels);
        noiseTexture->SetTextureParameter(TextureParameterType::WrapMode, TextureParameterValue::Repeat);

        Ref<Texture2D> positionTexture = mainFramebuffer->GetColorBuffer(GBuffer::CB_Position)->ToTexture();
        Ref<Texture2D> normalTexture = mainFramebuffer->GetColorBuffer(GBuffer::CB_Normal)->ToTexture();

        SsaoInput ssaoInput;
        ssaoInput.noiseScale = (float2)framebufferSize / float2(4.0f, 4.0f);

        for (u32 index = 0; index < 32; ++index)
        {
            vec3 sample(
                distributor(engine) * 2.0 - 1.0,
                distributor(engine) * 2.0 - 1.0,
                distributor(engine)
            );

            sample = glm::normalize(sample);
            sample *= distributor(engine);

            f32 scale = (f32)index / 32.0f;
            scale = glm::mix(0.1f, 1.0f, scale * scale);
            sample *= scale;

            ssaoInput.kernel[index] = vec4(sample, 0.0f);
        }

        ssaoMaterial->SetParameterBlock<SsaoInput>("SsaoInput", ssaoInput);
        ssaoMaterial->SetParameterValue<MaterialParameterTexture2D>("uPosition", positionTexture);
        ssaoMaterial->SetParameterValue<MaterialParameterTexture2D>("uNormal", normalTexture);
        ssaoMaterial->SetParameterValue<MaterialParameterTexture2D>("uNoise", noiseTexture);
    }

    {
        imageBlurMaterial->SetParameterValue<MaterialParameterTexture2D>("uTexture", ssaoFramebuffer->GetColorBuffer(GBuffer::CB_Albedo)->ToTexture());
    }

    // setup blend parameters
    {
        BlendInput blendInput;
        blendInput.screenSize = framebufferSize;
        blendInput.blendMode = vec2(1.0f, 0.0f);

        imageBlendMaterial->SetParameterBlock<BlendInput>("ScreenInfo", blendInput);
    }

    float2 screenQuadVertices[6] =
    {
        float2(-1.0f, -1.0f), float2(1.0f, -1.0f), float2(1.0f, 1.0f),
        float2(-1.0f, -1.0f), float2(1.0f,  1.0f), float2(-1.0f, 1.0f),
    };

    Ref<VertexBuffer> screenQuadBuffer = GetApiManager()->CreateVertexBuffer(6, sizeof(float2), screenQuadVertices);
    screenQuadBuffer->SetLayout({ { "aPosition", ShaderData::Float2 } });

    screenQuad = GetApiManager()->CreateVertexArray();
    screenQuad->AddBuffer(screenQuadBuffer);

    hasBeenInitialised = true;
}

struct ImageDataBuffer
{
    vec2 position;
    vec2 size;
};

struct ShadowLight
{
    vec3 position;
    f32 type;
    mat4 sourceMatrix;
};

struct UniformBufferShadowMatrices
{
    ShadowLight shadowCasters[16];
    int shadowCasterCount;
};

//////////////////////////////////////////////////////////////////////////

void RenderPassManager::RenderImpl()
{
    if (!hasBeenInitialised)
    {
        // we can't render anything yet
        return;
    }

    Ref<Camera> camera = Camera::GetActiveCamera();
    if (camera != nullptr)
    {
        Renderer::SetViewProjectionMatrix(camera->GetViewMatrix(), camera->GetProjectionMatrix());
    }

    // opaque & transparent pass
    Renderer::BeginScene(mainFramebuffer);
    {
        GlCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
        GlCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        // opaque pass
        renderPasses[RenderPassType::Opaque].Perform();

        // transparent pass
        renderPasses[RenderPassType::Transparent].Perform();
    }
    Renderer::EndScene();

    // particle pass
    Renderer::BeginScene(particleFramebuffer);
    {
        GlCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
        GlCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        particleFramebuffer->GetColorBuffer(GBuffer::CB_DepthStencil)->CopyAttachmentData(mainFramebuffer->GetColorBuffer(GBuffer::CB_DepthStencil));

        GlCall(glEnable(GL_BLEND));
        GlCall(glDepthMask(GL_FALSE));
        GlCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        renderPasses[RenderPassType::Particle].Perform();

        GlCall(glDisable(GL_BLEND));
        GlCall(glDepthMask(GL_TRUE));
    }
    Renderer::EndScene();

    // ssao pass
    // everything in the opaque and transparent pass weill be rendered again
    Renderer::BeginScene(ssaoFramebuffer);
    {
        GlCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
        GlCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        Renderer::Submit(ssaoMaterial, screenQuad);
    }
    Renderer::EndScene();

    Renderer::BeginScene(blurFramebuffer);
    {
        GlCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
        GlCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        Renderer::Submit(imageBlurMaterial, screenQuad);
    }
    Renderer::EndScene();

    // #todo - replace this with shadow casting lights from the light manager

    const f32 shadowSize = 20.0f;

    vec3 lightPosition = vec3(16.0f, 8.0f, 16.0f);
    fmat4 lightView = glm::lookAt(lightPosition, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    fmat4 lightProjection = glm::ortho(-shadowSize, shadowSize, -shadowSize, shadowSize, 0.5f, 40.0f);
    
    Renderer::SetViewProjectionMatrix(lightView, lightProjection);

    Renderer::BeginScene(lightFramebuffer);
    {
        GlCall(glClear(GL_DEPTH_BUFFER_BIT));
        GlCall(glCullFace(GL_FRONT));
        
        renderPasses[RenderPassType::Shadow].Perform();

        GlCall(glCullFace(GL_BACK));
    }
    Renderer::EndScene();

    UniformBufferShadowMatrices shadowMatrices;
    shadowMatrices.shadowCasterCount = 1;
    shadowMatrices.shadowCasters[0] = { lightPosition, 0.0f, lightProjection * lightView };

    if (camera != nullptr)
    {
        Renderer::SetViewProjectionMatrix(camera->GetViewMatrix(), camera->GetProjectionMatrix());
    }

    Renderer::BeginScene(sslFramebuffer);
    {
        GlCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        LightBuffer lights = GetLightManager()->GetBufferData();

        // #todo - replace with light manager data
        sslMaterial->SetParameterBlock<LightBuffer>("Lights", lights);
        sslMaterial->SetParameterBlock<UniformBufferShadowMatrices>("ShadowMatrices", shadowMatrices);

        sslMaterial->SetParameterValue<MaterialParameterTexture2D>("uAlbedo", mainFramebuffer->GetColorBuffer(GBuffer::CB_Albedo)->ToTexture());
        sslMaterial->SetParameterValue<MaterialParameterTexture2D>("uWorldPos", mainFramebuffer->GetColorBuffer(GBuffer::CB_Position)->ToTexture());
        sslMaterial->SetParameterValue<MaterialParameterTexture2D>("uObjectNormal", mainFramebuffer->GetColorBuffer(GBuffer::CB_Normal)->ToTexture());
        sslMaterial->SetParameterValue<MaterialParameterTexture2D>("uDirectionalShadowMap", lightFramebuffer->GetColorBuffer(GBuffer::CB_DepthStencil)->ToTexture());

        Renderer::Submit(sslMaterial, screenQuad);
    }
    Renderer::EndScene();

    // ssao + ssl blending
    Renderer::BeginScene(blendFramebuffer);
    {
        GlCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        BlendInput blendInput;
        blendInput.screenSize = framebufferSize;
        blendInput.blendMode = vec2(1.0f, 0.0f);

        imageBlendMaterial->SetParameterBlock<BlendInput>("ScreenInfo", blendInput);

		imageBlendMaterial->SetParameterValue<MaterialParameterTexture2D>("uTexture0", sslFramebuffer->GetColorBuffer(GBuffer::CB_Albedo)->ToTexture());
        imageBlendMaterial->SetParameterValue<MaterialParameterTexture2D>("uTexture1", blurFramebuffer->GetColorBuffer(GBuffer::CB_Albedo)->ToTexture());

        Renderer::Submit(imageBlendMaterial, screenQuad);
    }
    Renderer::EndScene();

    // blendBuffer + particle blending
    Renderer::BeginScene(blendFramebuffer);
    {
        GlCall(glClear(GL_DEPTH_BUFFER_BIT));

        BlendInput blendInput;
        blendInput.screenSize = framebufferSize;
        blendInput.blendMode = vec2(0.0f, 0.0f);

        imageBlendMaterial->SetParameterBlock<BlendInput>("ScreenInfo", blendInput);

        imageBlendMaterial->SetParameterValue<MaterialParameterTexture2D>("uTexture0", blendFramebuffer->GetColorBuffer(GBuffer::CB_Albedo)->ToTexture());
        imageBlendMaterial->SetParameterValue<MaterialParameterTexture2D>("uTexture1", particleFramebuffer->GetColorBuffer(GBuffer::CB_Albedo)->ToTexture());

        Renderer::Submit(imageBlendMaterial, screenQuad);
    }
    Renderer::EndScene();

    GlCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    ImageDataBuffer imageDataBuffer;
    imageDataBuffer.position = vec2(0.0f);
    imageDataBuffer.size = vec2(1.0f);

    imageRenderMaterial->SetParameterBlock<ImageDataBuffer>("ImageDataBuffer", imageDataBuffer);
    imageRenderMaterial->SetParameterValue<MaterialParameterTexture2D>("uTexture", blendFramebuffer->GetColorBuffer(GBuffer::CB_Albedo)->ToTexture());

    Renderer::Submit(imageRenderMaterial, screenQuad);

#if 0
    GlCall(glClear(GL_DEPTH_BUFFER_BIT));
    imageDataBuffer.position = vec2(-1.0f + 0.25f);
    //imageDataBuffer.position = vec2(0.0f);
    imageDataBuffer.size = vec2(0.25f);
    //imageDataBuffer.size = vec2(1.0f);

    imageRenderMaterial->SetParameterBlock<ImageDataBuffer>("ImageDataBuffer", imageDataBuffer);
    imageRenderMaterial->SetParameterValue<MaterialParameterTexture2D>("uTexture", lightFramebuffer->GetColorBuffer(GBuffer::CB_DepthStencil)->ToTexture());

    Renderer::Submit(imageRenderMaterial, screenQuad);
#endif
}

//////////////////////////////////////////////////////////////////////////

void RenderPassManager::RenderTask_Initialise()
{
}

//////////////////////////////////////////////////////////////////////////
