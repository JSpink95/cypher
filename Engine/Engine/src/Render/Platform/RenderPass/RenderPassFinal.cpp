//////////////////////////////////////////////////////////////////////////
//    File        	: RenderPassFinal.cpp
//    Created By    : Jack Spink
//    Created On 	: [28/01/2020]
//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassFinal.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Core.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Mesh.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Renderer.h"
#include "Render/Platform/Material.h"
#include "Render/Platform/ApiManager.h"
#include "Render/Platform/Framebuffer.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Utility/MeshLibrary.h"
#include "Render/Utility/TextureLibrary.h"
#include "Render/Utility/MaterialLibrary.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassManager.h"
#include "Render/Platform/RenderPass/RenderPassSSL.h"
#include "Render/Platform/RenderPass/RenderPassBloom.h"
#include "Render/Platform/RenderPass/RenderPassSSAO.h"
#include "Render/Platform/RenderPass/RenderPassUnlit.h"
#include "Render/Platform/RenderPass/RenderPassParticle.h"

//////////////////////////////////////////////////////////////////////////

#ifdef _OPENGL
    #include "glfw.h"
#endif

//////////////////////////////////////////////////////////////////////////

#define RENDER_SSAO_OUTPUT
#define RENDER_BLOOM_OUTPUT

//////////////////////////////////////////////////////////////////////////

struct OverlayInput
{
    vec2 framebufferSize;
    vec2 blendMode;
};

//////////////////////////////////////////////////////////////////////////

struct ImageOutput
{
    vec2 position;
    vec2 size;
};

//////////////////////////////////////////////////////////////////////////

RenderPassFinal::RenderPassFinal()
{

}

//////////////////////////////////////////////////////////////////////////

void RenderPassFinal::OnRenderCreate()
{
    Super::OnRenderCreate();

    FramebufferData fb;
    fb.resolution = (uint2)(float2(1280.0f, 720.0f) / 4.0f);
    fb.colorBuffers.at(GBuffer::CB_Albedo) = { true, false };

    litPassOverlay = GetApiManager()->CreateFramebuffer(fb);
    unlitPassOverlay = GetApiManager()->CreateFramebuffer(fb);
    particlePassOverlay = GetApiManager()->CreateFramebuffer(fb);

    overlay = MaterialLibrary::GetMaterial("assets:\\materials\\pp-overlay.xml");
    output = MaterialLibrary::GetMaterial("assets:\\materials\\pp-image-render.xml");
}

//////////////////////////////////////////////////////////////////////////

void RenderPassFinal::OnBegin()
{
    Super::OnBegin();
}

//////////////////////////////////////////////////////////////////////////

void RenderPassFinal::OnPerform()
{
    Super::OnPerform();

    Ref<RenderPassBase> sslPass = RenderPassManager::GetPass(RenderPassSSL::Id);
    Ref<RenderPassBase> bloomPass = RenderPassManager::GetPass(RenderPassBloom::Id);
    Ref<RenderPassBase> ssaoPass = RenderPassManager::GetPass(RenderPassSSAO::Id);
    Ref<RenderPassBase> unlitPass = RenderPassManager::GetPass(RenderPassUnlit::Id);
    Ref<RenderPassBase> particlePass = RenderPassManager::GetPass(RenderPassParticle::Id);

    Ref<Mesh> screen = MeshLibrary::GetMesh("engine:\\mesh\\screen-quad");

    if (screen != nullptr)
    {
        OverlayInput overlayInput;
        overlayInput.framebufferSize = float2(1280.0f, 720.0f) / 4.0f;
        overlayInput.blendMode = vec2(1.0f, 0.0f);
        overlay->SetParameterBlock<OverlayInput>("ScreenInfo", overlayInput);

        if (sslPass != nullptr && ssaoPass != nullptr)
        {
            Renderer::BeginScene(litPassOverlay);

            GlCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
            GlCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            overlay->SetParameterValue<MaterialParameterTexture2D>("uTexture0", bloomPass->GetAttachment(SSL::ColorOutput)->ToTexture());

            // #temp - disable ssao blending (use white texture for now)
            overlay->SetParameterValue<MaterialParameterTexture2D>("uTexture1", ssaoPass->GetAttachment(GBuffer::CB_Albedo)->ToTexture());
            //overlay->SetParameterValue<MaterialParameterTexture2D>("uTexture1", TextureLibrary::GetTexture("engine:\\textures\\white"));

            screen->Render(overlay, fmat4(1.0f));

            Renderer::EndScene();
        }

        overlayInput.blendMode = vec2(0.0f, 0.0f);
        overlay->SetParameterBlock<OverlayInput>("ScreenInfo", overlayInput);

        if (unlitPass != nullptr)
        {
            Renderer::BeginScene(unlitPassOverlay);

            GlCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
            GlCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            overlay->SetParameterValue<MaterialParameterTexture2D>("uTexture0", litPassOverlay->GetColorBuffer(GBuffer::CB_Albedo)->ToTexture());
            overlay->SetParameterValue<MaterialParameterTexture2D>("uTexture1", unlitPass->GetAttachment(GBuffer::CB_Albedo)->ToTexture());

            screen->Render(overlay, fmat4(1.0f));

            Renderer::EndScene();
        }

        if (particlePass != nullptr)
        {
            Renderer::BeginScene(particlePassOverlay);

            GlCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
            GlCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            overlay->SetParameterValue<MaterialParameterTexture2D>("uTexture0", unlitPassOverlay->GetColorBuffer(GBuffer::CB_Albedo)->ToTexture());
            overlay->SetParameterValue<MaterialParameterTexture2D>("uTexture1", particlePass->GetAttachment(GBuffer::CB_Albedo)->ToTexture());

            screen->Render(overlay, fmat4(1.0f));

            Renderer::EndScene();
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void RenderPassFinal::OnFinish()
{
    Super::OnFinish();

    Renderer::EndScene();

    // no framebuffer bound, render the final output to the screen.

    GlCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    Ref<Mesh> screen = MeshLibrary::GetMesh("engine:\\mesh\\screen-quad");
    if (screen != nullptr)
    {
        ImageOutput outputData;
        outputData.position = vec2(0.0f);
        outputData.size = vec2(1.0f);

        output->SetParameterBlock<ImageOutput>("ImageDataBuffer", outputData);
        output->SetParameterValue<MaterialParameterTexture2D>("uTexture", particlePassOverlay->GetColorBuffer(GBuffer::CB_Albedo)->ToTexture());
        
        screen->Render(output, fmat4(1.0f));

        f32 offset = 0.0f;

#ifdef RENDER_SSAO_OUTPUT
        {
            GlCall(glClear(GL_DEPTH_BUFFER_BIT));

            outputData.position = vec2(-0.74f + offset, -0.7f);
            outputData.size = vec2(0.25f);
            output->SetParameterBlock<ImageOutput>("ImageDataBuffer", outputData);

            Ref<RenderPassSSAO> bloom = RenderPassManager::GetPassAsType<RenderPassSSAO>();
            output->SetParameterValue<MaterialParameterTexture2D>("uTexture", bloom->GetAttachment(GBuffer::CB_Albedo)->ToTexture());

            screen->Render(output, fmat4(1.0f));

            offset += 0.5f;
        }
#endif

#ifdef RENDER_BLOOM_OUTPUT

        GlCall(glClear(GL_DEPTH_BUFFER_BIT));

        outputData.position = vec2(-0.74f + offset, -0.7f);
        outputData.size = vec2(0.25f);
        output->SetParameterBlock<ImageOutput>("ImageDataBuffer", outputData);

        Ref<RenderPassBloom> bloom = RenderPassManager::GetPassAsType<RenderPassBloom>();
        output->SetParameterValue<MaterialParameterTexture2D>("uTexture", bloom->GetAttachment(Bloom::Output)->ToTexture());

        screen->Render(output, fmat4(1.0f));

        offset += 0.5f;
#endif

    }
}

//////////////////////////////////////////////////////////////////////////
