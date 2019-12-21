//////////////////////////////////////////////////////////////////////////
//    File        	: Renderer.cpp
//    Created By    : Jack Spink
//    Created On 	: [18/9/2019]
//////////////////////////////////////////////////////////////////////////

#ifdef _OPENGL
    #include "glfw.h"
#endif // _OPENGL

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Renderer.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Core.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/StaticMeshObject.h"
#include "Render/Platform/Shader.h"
#include "Render/Platform/VertexArray.h"
#include "Render/Platform/MaterialParameter.h"
#include "Render/Platform/Framebuffer.h"

//////////////////////////////////////////////////////////////////////////

void Renderer::BeginScene(Ref<Framebuffer> framebuffer)
{
    if (framebuffer != nullptr)
    {
        framebuffer->Bind();
    }

    activeSceneData.framebuffer = framebuffer;
    // clear screen and all that jazz
}

//////////////////////////////////////////////////////////////////////////

void Renderer::EndScene()
{
    if (activeSceneData.framebuffer != nullptr)
    {
        activeSceneData.framebuffer->Unbind();
        activeSceneData.framebuffer = nullptr;
    }
}

//////////////////////////////////////////////////////////////////////////

void Renderer::SetViewMatrix(const fmat4& view)
{
    perFrameProperties.view = view;
    perFrameProperties.viewProjection = perFrameProperties.projection * view;
}

//////////////////////////////////////////////////////////////////////////

void Renderer::SetProjectionMatrix(const fmat4& projection)
{
    perFrameProperties.projection = projection;
    perFrameProperties.viewProjection = projection * perFrameProperties.view;
}

//////////////////////////////////////////////////////////////////////////

void Renderer::SetViewProjectionMatrix(const fmat4& view, const fmat4& projection)
{
    perFrameProperties.view = view;
    perFrameProperties.projection = projection;
    perFrameProperties.viewProjection = projection * view;
}

//////////////////////////////////////////////////////////////////////////

void Renderer::SetTimeAndDeltaTime(const f32 time, const f32 dt)
{
    timeProperties.time = time;
    timeProperties.deltaTime = dt;
}

//////////////////////////////////////////////////////////////////////////

void Renderer::Submit(Ref<Material> material, Ref<VertexArray> mesh, const fmat4& transform/* = fmat4(1.0f)*/)
{
    MaterialObjectProperties perObjectProperties;
    perObjectProperties.transform = transform;

    // 
    // upload the properties
    // 
    
    material->SetParameterBlock<MaterialFrameProperties>("PerFrame", perFrameProperties);
    material->SetParameterBlock<MaterialTimeProperties>("Time", timeProperties);
    material->SetParameterBlock<MaterialObjectProperties>("PerObject", perObjectProperties);

    // 
    // bind the properties
    // 

    material->BindProperties();

    mesh->Bind();
    GlCall(mesh->Draw(renderMode));
}

//////////////////////////////////////////////////////////////////////////

void Renderer::SetRenderMode(s32 const mode)
{
    renderMode = mode;
}

//////////////////////////////////////////////////////////////////////////

s32 Renderer::renderMode = GL_TRIANGLES;

//////////////////////////////////////////////////////////////////////////
// - RenderCommand

void RenderCommand::Clear(const s32 code)
{
    GlCall(glClear(code));
}

//////////////////////////////////////////////////////////////////////////
