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

RenderPassUnlit::RenderPassUnlit()
    : RenderPassBase(RenderPassUnlit::Id)
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
    fb.resolution = (uint2)(float2(1280.0f, 720.0f) / 4.0f);
    fb.colorBuffers.at(GBuffer::CB_Albedo) = { true, false };
    fb.colorBuffers.at(GBuffer::CB_DepthStencil) = { true, true };

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

    for (auto& it : opaqueObjects)
    {
        it.second->OnRender(RenderPassType::Unlit);
    }

    // #todo - sort transparent objects by distance to camera
    for (auto& it : transparentObjects)
    {
        it.second->OnRender(RenderPassType::Unlit);
    }
}

//////////////////////////////////////////////////////////////////////////

void RenderPassUnlit::OnFinish()
{
    Super::OnFinish();

    Renderer::EndScene();
}

//////////////////////////////////////////////////////////////////////////

void RenderPassUnlit::AddObjectToPass(Object* object, bool transparent/* = false*/)
{
    if (transparent)
    {
        transparentObjects.emplace(object->GetId(), object);
    }
    else
    {
        opaqueObjects.emplace(object->GetId(), object);
    }
}

//////////////////////////////////////////////////////////////////////////

void RenderPassUnlit::RemoveObjectFromPass(Object* object)
{
    auto removeIfPresent = [](const ObjectId& id, std::unordered_map<ObjectId, Object*>& objects) -> bool
    {
        auto it = objects.find(id);
        if (it == objects.end())
        {
            return false;
        }

        objects.erase(id);
        return true;
    };

    if (!removeIfPresent(object->GetId(), opaqueObjects))
    {
        removeIfPresent(object->GetId(), transparentObjects);
    }
}

//////////////////////////////////////////////////////////////////////////
