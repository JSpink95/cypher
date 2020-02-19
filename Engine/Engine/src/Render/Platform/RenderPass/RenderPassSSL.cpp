//////////////////////////////////////////////////////////////////////////
//    File        	: RenderPassSSL.cpp
//    Created By    : Jack Spink
//    Created On 	: [28/01/2020]
//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassSSL.h"

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
#include "Render/Utility/MaterialLibrary.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Effects/LightManager.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassLit.h"
#include "Render/Platform/RenderPass/RenderPassShadow.h"
#include "Render/Platform/RenderPass/RenderPassManager.h"

//////////////////////////////////////////////////////////////////////////

#ifdef _OPENGL
    #include "glfw.h"
#endif

//////////////////////////////////////////////////////////////////////////

const GBuffer::Color SSL::ColorOutput = GBuffer::CB_Albedo;

//////////////////////////////////////////////////////////////////////////

RenderPassSSL::RenderPassSSL()
{

}

//////////////////////////////////////////////////////////////////////////

Ref<FramebufferAttachment> RenderPassSSL::GetAttachment(GBuffer::Color attachment)
{
    return framebuffer->GetColorBuffer(attachment);
}

//////////////////////////////////////////////////////////////////////////
   
void RenderPassSSL::OnRenderCreate()
{
    Super::OnRenderCreate();

    FramebufferData fb;
    fb.resolution = RenderPassManager::GetFramebufferSize();
    fb.colorBuffers.at(SSL::ColorOutput) = { true, true };

    framebuffer = GetApiManager()->CreateFramebuffer(fb);

    ssl = MaterialLibrary::GetMaterial("assets:\\materials\\pp-ssl.xml")->Clone();
}

//////////////////////////////////////////////////////////////////////////

void RenderPassSSL::OnBegin()
{
    Super::OnBegin();

    Renderer::BeginScene(framebuffer);

    GlCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
    GlCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

//////////////////////////////////////////////////////////////////////////

void RenderPassSSL::OnPerform()
{
    Super::OnPerform();

    Ref<Mesh> screen = MeshLibrary::GetMesh("engine:\\mesh\\screen-quad");
    Ref<RenderPassBase> litPass = RenderPassManager::GetPass(RenderPassLit::Id);
    Ref<RenderPassShadow> shadowPass = RenderPassManager::GetPassAsType<RenderPassShadow>(RenderPassShadow::Id);

    if (screen != nullptr && litPass != nullptr && shadowPass != nullptr)
    {
        LightBuffer lights = GetLightManager()->GetBufferData();

        ShadowBufferData shadowData;
        shadowPass->GetShadowBufferData(shadowData);

        ssl->SetParameterBlock<LightBuffer>("Lights", lights);
        ssl->SetParameterBlock<ShadowBufferData>("ShadowMatrices", shadowData);

        ssl->SetParameterValue<MaterialParameterTexture2D>("uAlbedo", litPass->GetAttachment(GBuffer::CB_Albedo)->ToTexture());
        ssl->SetParameterValue<MaterialParameterTexture2D>("uWorldPos", litPass->GetAttachment(GBuffer::CB_Position)->ToTexture());
        ssl->SetParameterValue<MaterialParameterTexture2D>("uObjectNormal", litPass->GetAttachment(GBuffer::CB_Normal)->ToTexture());
        ssl->SetParameterValue<MaterialParameterTexture2D>("uDirectionalShadowMap", shadowPass->GetAttachment(GBuffer::CB_DepthStencil)->ToTexture());

        screen->Render(ssl, fmat4(1.0f));
    }
}

//////////////////////////////////////////////////////////////////////////

void RenderPassSSL::OnFinish()
{
    Super::OnFinish();

    Renderer::EndScene();
}

//////////////////////////////////////////////////////////////////////////
