//////////////////////////////////////////////////////////////////////////
//    File        	: RenderPassParticle.cpp
//    Created By    : Jack Spink
//    Created On 	: [27/01/2020]
//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassParticle.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Object.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Renderer.h"
#include "Render/Platform/ApiManager.h"
#include "Render/Platform/Framebuffer.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassUnlit.h"

//////////////////////////////////////////////////////////////////////////

#ifdef _OPENGL
    #include "glfw.h"
#endif

//////////////////////////////////////////////////////////////////////////

RenderPassParticle::RenderPassParticle()
{

}

//////////////////////////////////////////////////////////////////////////

Ref<FramebufferAttachment> RenderPassParticle::GetAttachment(GBuffer::Color attachment)
{
    return framebuffer->GetColorBuffer(attachment);
}

//////////////////////////////////////////////////////////////////////////

void RenderPassParticle::OnRenderCreate()
{
    Super::OnRenderCreate();

    FramebufferData fb;
    fb.resolution = (uint2)(float2(1024.0f, 720.0f) / 4.0f);
    fb.colorBuffers.at(GBuffer::CB_Albedo) = { true, false };
    fb.colorBuffers.at(GBuffer::CB_DepthStencil) = { true, true };

    framebuffer = GetApiManager()->CreateFramebuffer(fb);
}

//////////////////////////////////////////////////////////////////////////

void RenderPassParticle::OnBegin()
{
    Super::OnBegin();

    Renderer::BeginScene(framebuffer);

    // #todo - add abstracted render commands for this kind of stuff
    GlCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
    GlCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    GlCall(glEnable(GL_BLEND));
    GlCall(glDepthMask(GL_FALSE));
    GlCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    if (Ref<RenderPassBase> unlitPass = RenderPassManager::GetPass(RenderPassUnlit::Id))
    {
        // copy across the depth from the unlit pass.
        // this way, particles have access to the same depth buffer that all previously rendered objects have written to (lit + unlit).
        Ref<FramebufferAttachment> depthAttachment = unlitPass->GetAttachment(GBuffer::CB_DepthStencil);
        framebuffer->GetColorBuffer(GBuffer::CB_DepthStencil)->CopyAttachmentData(depthAttachment);
    }
}

//////////////////////////////////////////////////////////////////////////

void RenderPassParticle::OnPerform()
{
    Super::OnPerform();

    for (RenderFunction* function : renderFunctions)
    {
        if (function && function->enabled)
        {
            function->ExecuteRender(RenderPassType::Particle, nullptr);
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void RenderPassParticle::OnFinish()
{
    Super::OnFinish();

    GlCall(glDisable(GL_BLEND));
    GlCall(glDepthMask(GL_TRUE));
    Renderer::EndScene();
}

//////////////////////////////////////////////////////////////////////////

void RenderPassParticle::AddRenderFunction(RenderFunction* function)
{
    renderFunctions.push_back(function);
}

//////////////////////////////////////////////////////////////////////////

void RenderPassParticle::RemoveRenderFunction(RenderFunction* function)
{
    renderFunctions.erase(std::remove(renderFunctions.begin(), renderFunctions.end(), function), renderFunctions.end());
}

//////////////////////////////////////////////////////////////////////////
