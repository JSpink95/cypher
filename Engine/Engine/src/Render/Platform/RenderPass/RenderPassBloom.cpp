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

#include "Render/Platform/RenderPass/RenderPassSSL.h"
#include "Render/Platform/RenderPass/RenderPassManager.h"

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

    guassianBlurMaterial = MaterialLibrary::GetMaterial("assets:\\materials\\pp-guassian-blur.xml");
    bloomBlendMaterial = MaterialLibrary::GetMaterial("assets:\\materials\\pp-bloom.xml");

    FramebufferData fbInit;
    fbInit.resolution = RenderPassManager::GetFramebufferSize();
    fbInit.colorBuffers.at(Bloom::Output) = { true, true };

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
    Ref<RenderPassSSL> ssl = RenderPassManager::GetPassAsType<RenderPassSSL>(RenderPassSSL::Id);

    if (guassianBlurMaterial != nullptr && bloomBlendMaterial != nullptr)
    {
        GuassianBlurData blur = { false };
        BloomData bloomData = { 1.0f };

        const u32 blendInterations = 4u;
        for (u32 idx = 0u; idx < blendInterations; ++idx)
        {
            const bool firstIteration = (idx == 0u);

            Renderer::BeginScene(framebuffers[blur.horizontal]);

            GlCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
            GlCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            guassianBlurMaterial->SetParameterBlock("GuassianData", blur);
            guassianBlurMaterial->SetParameterValue<MaterialParameterTexture2D>(
                "uTexture",
                firstIteration ? ssl->GetAttachment(SSL::BloomOutput)->ToTexture() : framebuffers[!blur.horizontal]->GetColorBuffer(Bloom::Output)->ToTexture()
            );

            screen->Render(guassianBlurMaterial, fmat4(1.0f));

            Renderer::EndScene();

            blur.horizontal = !blur.horizontal;
        }

        Renderer::BeginScene(bloom);

        GlCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
        GlCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        bloomBlendMaterial->SetParameterBlock("BloomData", bloomData);
        bloomBlendMaterial->SetParameterValue<MaterialParameterTexture2D>("uBloomBlur", framebuffers[!blur.horizontal]->GetColorBuffer(Bloom::Output)->ToTexture());
        bloomBlendMaterial->SetParameterValue<MaterialParameterTexture2D>("uScene", ssl->GetAttachment(SSL::ColorOutput)->ToTexture());

        screen->Render(bloomBlendMaterial, fmat4(1.0f));

        Renderer::EndScene();
    }
}

//////////////////////////////////////////////////////////////////////////

void RenderPassBloom::OnFinish()
{
    Super::OnFinish();
}

//////////////////////////////////////////////////////////////////////////
