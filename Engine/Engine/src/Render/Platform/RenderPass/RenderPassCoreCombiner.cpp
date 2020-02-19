//////////////////////////////////////////////////////////////////////////
//    File        	: RenderPassCoreCombiner.cpp
//    Created By    : Jack Spink
//    Created On 	: [19/02/2020]
//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassCoreCombiner.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Core.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Mesh.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/ApiManager.h"
#include "Render/Platform/Framebuffer.h"
#include "Render/Platform/Renderer.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassManager.h"
#include "Render/Platform/RenderPass/RenderPassSSL.h"
#include "Render/Platform/RenderPass/RenderPassSSAO.h"
#include "Render/Platform/RenderPass/RenderPassUnlit.h"
#include "Render/Platform/RenderPass/RenderPassParticle.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Utility/MeshLibrary.h"
#include "Render/Utility/MaterialLibrary.h"

//////////////////////////////////////////////////////////////////////////

#ifdef _OPENGL
    #include "glfw.h"
#endif

//////////////////////////////////////////////////////////////////////////

struct OverlayInput
{
    vec2 framebufferSize;
    vec2 blendMode;
};

// 
// OverlayInput::blendMode
// x = 0 -> LinearBlend
// x = 1 -> MultiplyBlend
// 

//////////////////////////////////////////////////////////////////////////

RenderPassCoreCombiner::RenderPassCoreCombiner()
{

}

//////////////////////////////////////////////////////////////////////////

Ref<FramebufferAttachment> RenderPassCoreCombiner::GetAttachment(GBuffer::Color attachment)
{
    return overlayParticle->GetColorBuffer(attachment);
}

//////////////////////////////////////////////////////////////////////////

void RenderPassCoreCombiner::OnRenderCreate()
{
    Super::OnRenderCreate();

    FramebufferData fb;
    fb.resolution = RenderPassManager::GetFramebufferSize();
    fb.colorBuffers.at(GBuffer::CB_Albedo) = { true, true };

    overlaySSAO = GetApiManager()->CreateFramebuffer(fb);
    overlayUnlit = GetApiManager()->CreateFramebuffer(fb);
    overlayParticle = GetApiManager()->CreateFramebuffer(fb);

    overlay = MaterialLibrary::GetMaterial("assets:\\materials\\pp-overlay.xml");
}

//////////////////////////////////////////////////////////////////////////

void RenderPassCoreCombiner::OnBegin()
{
    Super::OnBegin();
}

//////////////////////////////////////////////////////////////////////////

void RenderPassCoreCombiner::OnPerform()
{
    Super::OnPerform();

    Ref<RenderPassBase> sslPass = RenderPassManager::GetPass(RenderPassSSL::Id);
    Ref<RenderPassBase> ssaoPass = RenderPassManager::GetPass(RenderPassSSAO::Id);
    Ref<RenderPassBase> unlitPass = RenderPassManager::GetPass(RenderPassUnlit::Id);
    Ref<RenderPassBase> particlePass = RenderPassManager::GetPass(RenderPassParticle::Id);

    Ref<Mesh> screen = MeshLibrary::GetMesh("engine:\\mesh\\screen-quad");

    if (screen != nullptr)
    {
        // Combine (SSL + SSAO)

        if (sslPass != nullptr && ssaoPass != nullptr)
        {
            OverlayInput overlayInput;
            overlayInput.framebufferSize = RenderPassManager::GetFramebufferSize();
            overlayInput.blendMode = vec2(1.0f, 0.0f);

            Renderer::BeginScene(overlaySSAO);

            GlCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
            GlCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            overlay->SetParameterBlock<OverlayInput>("ScreenInfo", overlayInput);
            overlay->SetParameterValue<MaterialParameterTexture2D>("uTexture0", sslPass->GetAttachment(SSL::ColorOutput)->ToTexture());
            overlay->SetParameterValue<MaterialParameterTexture2D>("uTexture1", ssaoPass->GetAttachment(GBuffer::CB_Albedo)->ToTexture());
            
            screen->Render(overlay, fmat4(1.0f));

            Renderer::EndScene();
        }

        // Combine (SSL + SSAO) + Unlit

        if (sslPass != nullptr && unlitPass != nullptr)
        {
            OverlayInput overlayInput;
            overlayInput.framebufferSize = RenderPassManager::GetFramebufferSize();
            overlayInput.blendMode = vec2(0.0f, 0.0f);

            Renderer::BeginScene(overlayUnlit);

            GlCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
            GlCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            overlay->SetParameterBlock<OverlayInput>("ScreenInfo", overlayInput);
            overlay->SetParameterValue<MaterialParameterTexture2D>("uTexture0", overlaySSAO->GetColorBuffer(GBuffer::CB_Albedo)->ToTexture());
            overlay->SetParameterValue<MaterialParameterTexture2D>("uTexture1", unlitPass->GetAttachment(Unlit::ColorOutput)->ToTexture());

            screen->Render(overlay, fmat4(1.0f));

            Renderer::EndScene();
        }

        // Combine ((SSL + SSAO) + Unlit) + Particle

        if (particlePass != nullptr)
        {
            OverlayInput overlayInput;
            overlayInput.framebufferSize = RenderPassManager::GetFramebufferSize();
            overlayInput.blendMode = vec2(0.0f, 0.0f);

            Renderer::BeginScene(overlayParticle);

            GlCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
            GlCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            overlay->SetParameterBlock<OverlayInput>("ScreenInfo", overlayInput);
            overlay->SetParameterValue<MaterialParameterTexture2D>("uTexture0", overlayUnlit->GetColorBuffer(GBuffer::CB_Albedo)->ToTexture());
            overlay->SetParameterValue<MaterialParameterTexture2D>("uTexture1", particlePass->GetAttachment(GBuffer::CB_Albedo)->ToTexture());

            screen->Render(overlay, fmat4(1.0f));

            Renderer::EndScene();
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void RenderPassCoreCombiner::OnFinish()
{
    Super::OnFinish();
}

//////////////////////////////////////////////////////////////////////////
