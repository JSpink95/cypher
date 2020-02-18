//////////////////////////////////////////////////////////////////////////
//    File        	: RenderPassLit.cpp
//    Created By    : Jack Spink
//    Created On 	: [27/01/2020]
//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassLit.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Object.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/ApiManager.h"
#include "Render/Platform/Framebuffer.h"
#include "Render/Platform/Renderer.h"

//////////////////////////////////////////////////////////////////////////

#ifdef _OPENGL
    #include "glfw.h"
#endif

//////////////////////////////////////////////////////////////////////////

const GBuffer::Color Lit::ColorOutput = GBuffer::CB_Albedo;
const GBuffer::Color Lit::PositionOutput = GBuffer::CB_Position;
const GBuffer::Color Lit::NormalOutput = GBuffer::CB_Normal;
const GBuffer::Color Lit::DepthOutput = GBuffer::CB_DepthStencil;

//////////////////////////////////////////////////////////////////////////

RenderPassLit::RenderPassLit()
{

}

//////////////////////////////////////////////////////////////////////////

Ref<FramebufferAttachment> RenderPassLit::GetAttachment(GBuffer::Color attachment)
{
    return framebuffer->GetColorBuffer(attachment);
}

//////////////////////////////////////////////////////////////////////////

void RenderPassLit::OnRenderCreate()
{
    Super::OnRenderCreate();

    // #todo - remove hardcoded screen dimensions

    FramebufferData fb;
    fb.resolution = (uint2)(float2(1280.0f, 720.0f) / 4.0f);
    fb.colorBuffers.at(Lit::ColorOutput) = { true, true };
    fb.colorBuffers.at(Lit::PositionOutput) = { true, true };
    fb.colorBuffers.at(Lit::NormalOutput) = { true, true };
    fb.colorBuffers.at(Lit::DepthOutput) = { true, true };

    framebuffer = GetApiManager()->CreateFramebuffer(fb);
}

//////////////////////////////////////////////////////////////////////////

void RenderPassLit::OnBegin()
{
    Super::OnBegin();

    Renderer::BeginScene(framebuffer);

    // #todo - add abstracted render commands for this kind of stuff
    GlCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
    GlCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

//////////////////////////////////////////////////////////////////////////

void RenderPassLit::OnPerform()
{
    Super::OnPerform();

    for (RenderFunction* function : opaqueRenderFunctions)
    {
        if (function && function->enabled)
        {
            function->ExecuteRender(RenderPassType::Lit, nullptr);
        }
    }

    // #todo - sort transparent objects by distance to camera
    for (RenderFunction* function : transparentRenderFunctions)
    {
        if (function && function->enabled)
        {
            function->ExecuteRender(RenderPassType::Lit, nullptr);
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void RenderPassLit::OnFinish()
{
    Super::OnFinish();

    Renderer::EndScene();
}

//////////////////////////////////////////////////////////////////////////

void RenderPassLit::AddRenderFunction(RenderFunction* function, bool transparent/* = false*/)
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

void RenderPassLit::RemoveRenderFunction(RenderFunction* function, bool transparent/* = false*/)
{
    static auto removeFromVector = [](std::vector<RenderFunction*>& vector, RenderFunction* function) -> bool
    {
        auto it = vector.erase(std::remove(vector.begin(), vector.end(), function), vector.end());
        return true;
    };

    removeFromVector(transparent ? transparentRenderFunctions : opaqueRenderFunctions, function);
}

//////////////////////////////////////////////////////////////////////////
