//////////////////////////////////////////////////////////////////////////
//    File        	: RenderPassBloom.cpp
//    Created By    : Jack Spink
//    Created On 	: [17/02/2020]
//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassBloom.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Core.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Utility/MaterialLibrary.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Renderer.h"
#include "Render/Platform/ApiManager.h"
#include "Render/Platform/Material.h"
#include "Render/Platform/Framebuffer.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassManager.h"
#include "Render/Platform/RenderPass/RenderPassCoreCombiner.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Mesh.h"
#include "Render/Utility/MeshLibrary.h"

//////////////////////////////////////////////////////////////////////////

#ifdef _OPENGL
    #include "glfw.h"
#endif

//////////////////////////////////////////////////////////////////////////

const GBuffer::Color Bloom::Output = GBuffer::CB_Albedo;

//////////////////////////////////////////////////////////////////////////

RenderPassBloom::RenderPassBloom()
{

}

//////////////////////////////////////////////////////////////////////////

void RenderPassBloom::OnRenderCreate()
{
    Super::OnRenderCreate();

    extractBloomMaterial = MaterialLibrary::GetMaterial("assets:\\materials\\pp-extract-bloom.xml");
    guassianBlurMaterial = MaterialLibrary::GetMaterial("assets:\\materials\\pp-guassian-blur.xml");
    bloomBlendMaterial = MaterialLibrary::GetMaterial("assets:\\materials\\pp-bloom.xml");

    FramebufferData fbInit;
    fbInit.resolution = RenderPassManager::GetFramebufferSize();
    fbInit.colorBuffers.at(Bloom::Output) = { true, true };

    extractBloom = GetApiManager()->CreateFramebuffer(fbInit);

    framebuffers[0] = GetApiManager()->CreateFramebuffer(fbInit);
    framebuffers[1] = GetApiManager()->CreateFramebuffer(fbInit);

    bloom = GetApiManager()->CreateFramebuffer(fbInit);
}

//////////////////////////////////////////////////////////////////////////

Ref<FramebufferAttachment> RenderPassBloom::GetAttachment(GBuffer::Color attachment)
{
    return bloom->GetColorBuffer(attachment);
}

//////////////////////////////////////////////////////////////////////////

void RenderPassBloom::OnBegin()
{
    Super::OnBegin();
}

//////////////////////////////////////////////////////////////////////////

void RenderPassBloom::OnPerform()
{
    struct GuassianBlurData
    {
        bool horizontal;
    };

    struct BloomData
    {
        float exposure;
    };

    Super::OnPerform();

    Ref<Mesh> screen = MeshLibrary::GetMesh("engine:\\mesh\\screen-quad");
    Ref<RenderPassCoreCombiner> coreCombinerPass = RenderPassManager::GetPassAsType<RenderPassCoreCombiner>();

    if (screen != nullptr)
    {
        // extract bloom
        if (coreCombinerPass != nullptr)
        {
            Renderer::BeginScene(extractBloom);

            GlCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
            GlCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            extractBloomMaterial->SetParameterValue<MaterialParameterTexture2D>("uTexture", coreCombinerPass->GetAttachment(GBuffer::CB_Albedo)->ToTexture());

            screen->Render(extractBloomMaterial, fmat4(1.0f));

            Renderer::EndScene();
        }

        // guassian blur the bloom
        Ref<Texture2D> blurredResult = nullptr;
        if (guassianBlurMaterial != nullptr)
        {
            GuassianBlurData blur = { false };

            const u32 blendInterations = 10u;
            for (u32 idx = 0u; idx < blendInterations; ++idx)
            {
                const bool firstIteration = (idx == 0u);

                Renderer::BeginScene(framebuffers[blur.horizontal]);

                GlCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
                GlCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

                Ref<Texture2D> texture = firstIteration
                    ? extractBloom->GetColorBuffer(GBuffer::CB_Albedo)->ToTexture()
                    : framebuffers[!blur.horizontal]->GetColorBuffer(Bloom::Output)->ToTexture();

                guassianBlurMaterial->SetParameterBlock("GuassianData", blur);
                guassianBlurMaterial->SetParameterValue<MaterialParameterTexture2D>("uTexture", texture);

                screen->Render(guassianBlurMaterial, fmat4(1.0f));

                Renderer::EndScene();

                blur.horizontal = !blur.horizontal;
            }

            blurredResult = framebuffers[!blur.horizontal]->GetColorBuffer(Bloom::Output)->ToTexture();
        }

        if (bloomBlendMaterial != nullptr)
        {
            BloomData bloomData = { 0.5f };

            Renderer::BeginScene(bloom);

            GlCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
            GlCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            bloomBlendMaterial->SetParameterBlock("BloomData", bloomData);
            bloomBlendMaterial->SetParameterValue<MaterialParameterTexture2D>("uBloomBlur", blurredResult);
            bloomBlendMaterial->SetParameterValue<MaterialParameterTexture2D>("uScene", coreCombinerPass->GetAttachment(GBuffer::CB_Albedo)->ToTexture());

            screen->Render(bloomBlendMaterial, fmat4(1.0f));

            Renderer::EndScene();
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void RenderPassBloom::OnFinish()
{
    Super::OnFinish();
}

//////////////////////////////////////////////////////////////////////////
