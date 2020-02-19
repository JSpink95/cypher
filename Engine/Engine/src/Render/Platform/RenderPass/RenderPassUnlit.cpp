//////////////////////////////////////////////////////////////////////////
//    File        	: RenderPassUnlit.cpp
//    Created By    : Jack Spink
//    Created On 	: [27/01/2020]
//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassUnlit.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Object.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/ApiManager.h"
#include "Render/Platform/Framebuffer.h"
#include "Render/Platform/Renderer.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassLit.h"

//////////////////////////////////////////////////////////////////////////

#ifdef _OPENGL
    #include "glfw.h"
#endif

//////////////////////////////////////////////////////////////////////////

const GBuffer::Color Unlit::ColorOutput = GBuffer::CB_Albedo;
const GBuffer::Color Unlit::DepthOutput = GBuffer::CB_DepthStencil;

//////////////////////////////////////////////////////////////////////////

RenderPassUnlit::RenderPassUnlit()
{

}

//////////////////////////////////////////////////////////////////////////

Ref<FramebufferAttachment> RenderPassUnlit::GetAttachment(GBuffer::Color attachment)
{
    return framebuffer->GetColorBuffer(attachment);
}

//////////////////////////////////////////////////////////////////////////

void RenderPassUnlit::OnRenderCreate()
{
    Super::OnRenderCreate();

    // #todo - remove hardcoded screen dimensions

    FramebufferData fb;
    fb.resolution = RenderPassManager::GetFramebufferSize();
    fb.colorBuffers.at(Unlit::ColorOutput) = { true, true };
    fb.colorBuffers.at(Unlit::DepthOutput) = { true, true };

    framebuffer = GetApiManager()->CreateFramebuffer(fb);
}

//////////////////////////////////////////////////////////////////////////

void RenderPassUnlit::OnBegin()
{
    Super::OnBegin();

    Renderer::BeginScene(framebuffer);

    // #todo - add abstracted render commands for this kind of stuff
    GlCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
    GlCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    if (Ref<RenderPassBase> litPass = RenderPassManager::GetPass(RenderPassLit::Id))
    {
        // copy across the depth from the lit pass.
        // this allows us to "cheat" later on by simply pasting the unlit colorbuffer ontop of the final lighting pass
        Ref<FramebufferAttachment> depthAttachment = litPass->GetAttachment(GBuffer::CB_DepthStencil);
        framebuffer->GetColorBuffer(GBuffer::CB_DepthStencil)->CopyAttachmentData(depthAttachment);
    }
}

//////////////////////////////////////////////////////////////////////////

void RenderPassUnlit::OnPerform()
{
    Super::OnPerform();

    for (RenderFunction* function : opaqueRenderFunctions)
    {
        if (function && function->enabled)
        {
            function->ExecuteRender(RenderPassType::Unlit, nullptr);
        }
    }

    // #todo - sort transparent objects by distance to camera
    for (RenderFunction* function : transparentRenderFunctions)
    {
        if (function && function->enabled)
        {
            function->ExecuteRender(RenderPassType::Unlit, nullptr);
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void RenderPassUnlit::OnFinish()
{
    Super::OnFinish();

    Renderer::EndScene();
}

//////////////////////////////////////////////////////////////////////////

void RenderPassUnlit::AddRenderFunction(RenderFunction* function, bool transparent/* = false*/)
{
    if (transparent)
    {
        transparentRenderFunctions.push_back(function);
    }
    else
    {
        opaqueRenderFunctions.push_back(function);
    }
}

//////////////////////////////////////////////////////////////////////////

void RenderPassUnlit::RemoveRenderFunction(RenderFunction* function, bool transparent/* = false*/)
{
    static auto removeFromVector = [](std::vector<RenderFunction*>& vector, RenderFunction* function) -> bool
    {
        auto it = vector.erase(std::remove(vector.begin(), vector.end(), function), vector.end());
        return true;
    };

    removeFromVector(transparent ? transparentRenderFunctions : opaqueRenderFunctions, function);
}

//////////////////////////////////////////////////////////////////////////
