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

RenderPassLit::RenderPassLit()
    : RenderPassBase(RenderPassLit::Id)
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
    fb.colorBuffers.at(GBuffer::CB_Albedo) = { true, false };
    fb.colorBuffers.at(GBuffer::CB_Position) = { true, true };
    fb.colorBuffers.at(GBuffer::CB_Normal) = { true, true };
    fb.colorBuffers.at(GBuffer::CB_DepthStencil) = { true, true };

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

    for (auto& it : opaqueObjects)
    {
        it.second->OnRender(RenderPassType::Lit);
    }

    // #todo - sort transparent objects by distance to camera
    for (auto& it : transparentObjects)
    {
        it.second->OnRender(RenderPassType::Lit);
    }
}

//////////////////////////////////////////////////////////////////////////

void RenderPassLit::OnFinish()
{
    Super::OnFinish();

    Renderer::EndScene();
}

//////////////////////////////////////////////////////////////////////////

void RenderPassLit::AddObjectToPass(Object* object, bool transparent/* = false*/)
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

void RenderPassLit::RemoveObjectFromPass(Object* object)
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
