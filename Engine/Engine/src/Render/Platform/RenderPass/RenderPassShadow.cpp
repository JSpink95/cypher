//////////////////////////////////////////////////////////////////////////
//    File        	: RenderPassShadow.cpp
//    Created By    : Jack Spink
//    Created On 	: [27/01/2020]
//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassShadow.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Object.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Renderer.h"
#include "Render/Platform/ApiManager.h"
#include "Render/Platform/Framebuffer.h"

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Camera/Camera.h"

//////////////////////////////////////////////////////////////////////////

#ifdef _OPENGL
    #include "glfw.h"
#endif

//////////////////////////////////////////////////////////////////////////

static const uint2 ShadowMapSize = uint2(1024u);
static const f32 ShadowProjectionSize = 20.0f;

//////////////////////////////////////////////////////////////////////////

RenderPassShadow::RenderPassShadow()
    : RenderPassBase(RenderPassShadow::Id)
{

}

//////////////////////////////////////////////////////////////////////////

Ref<FramebufferAttachment> RenderPassShadow::GetAttachment(GBuffer::Color attachment)
{
    return framebuffer->GetColorBuffer(attachment);
}

//////////////////////////////////////////////////////////////////////////

void RenderPassShadow::OnRenderCreate()
{
    Super::OnRenderCreate();

    FramebufferData fb;
    fb.resolution = ShadowMapSize;
    fb.colorBuffers.at(GBuffer::CB_DepthStencil) = { true, true };

    framebuffer = GetApiManager()->CreateFramebuffer(fb);
}

//////////////////////////////////////////////////////////////////////////

void RenderPassShadow::OnBegin()
{
    Super::OnBegin();

    Renderer::BeginScene(framebuffer);

    // #todo - add abstracted render commands for this kind of stuff
    GlCall(glClear(GL_DEPTH_BUFFER_BIT));
    GlCall(glCullFace(GL_FRONT));

    // #todo - replace this with shadow casting lights from the light manager

    const f32 shadowSize = 20.0f;

    vec3 lightPosition = vec3(16.0f, 8.0f, 16.0f);
    fmat4 lightView = glm::lookAt(lightPosition, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    fmat4 lightProjection = glm::ortho(-shadowSize, shadowSize, -shadowSize, shadowSize, 0.5f, 40.0f);

    Renderer::SetViewProjectionMatrix(lightView, lightProjection);
}

//////////////////////////////////////////////////////////////////////////

void RenderPassShadow::OnPerform()
{
    Super::OnPerform();

    for (auto& it: lights)
    {
        fmat4 lightView = glm::lookAt(it.second, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
        fmat4 lightProjection = glm::ortho(-ShadowProjectionSize, ShadowProjectionSize, -ShadowProjectionSize, ShadowProjectionSize, 0.5f, 40.0f);

        Renderer::SetViewProjectionMatrix(lightView, lightProjection);

        for (auto& it : objects)
        {
            it.second->OnRender(RenderPassType::Shadow);
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void RenderPassShadow::OnFinish()
{
    Super::OnFinish();

    // set the camera matrices back to normal
    if (CameraBase* camera = CameraBase::GetActiveCamera())
    {
        Renderer::SetViewProjectionMatrix(camera->GetViewMatrix(), camera->GetProjectionMatrix());
    }

    GlCall(glCullFace(GL_BACK));
    Renderer::EndScene();
}

//////////////////////////////////////////////////////////////////////////

void RenderPassShadow::AddObjectToPass(Object* object)
{
    objects.emplace(object->GetId(), object);
}

//////////////////////////////////////////////////////////////////////////

void RenderPassShadow::RemoveObjectFromPass(Object* object)
{
    objects.erase(object->GetId());
}

//////////////////////////////////////////////////////////////////////////

void RenderPassShadow::AddDirectionalLight(const std::string& id, const float3& direction, const f32 distance)
{
    lights.emplace(id, direction * distance);
}

//////////////////////////////////////////////////////////////////////////

void RenderPassShadow::GetShadowBufferData(ShadowBufferData& output)
{
    output.shadowCasterCount = lights.size();

    u32 idx = 0;
    for (auto& it: lights)
    {
        const float3& light = it.second;

        fmat4 lightView = glm::lookAt(light, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
        fmat4 lightProjection = glm::ortho(-ShadowProjectionSize, ShadowProjectionSize, -ShadowProjectionSize, ShadowProjectionSize, 0.5f, 40.0f);

        output.shadowCasters[idx++] = { light, 0.0f, lightProjection * lightView };
    }
}

//////////////////////////////////////////////////////////////////////////
