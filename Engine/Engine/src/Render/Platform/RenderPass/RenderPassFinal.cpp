//////////////////////////////////////////////////////////////////////////
//    File        	: RenderPassFinal.cpp
//    Created By    : Jack Spink
//    Created On 	: [28/01/2020]
//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassFinal.h"

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
#include "Render/Utility/TextureLibrary.h"
#include "Render/Utility/MaterialLibrary.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassManager.h"
#include "Render/Platform/RenderPass/RenderPassBloom.h"

//////////////////////////////////////////////////////////////////////////

#ifdef _OPENGL
    #include "glfw.h"
#endif

//////////////////////////////////////////////////////////////////////////

//#define RENDER_SSAO_OUTPUT
//#define RENDER_BLOOM_OUTPUT

//////////////////////////////////////////////////////////////////////////

struct ImageOutput
{
    vec2 position;
    vec2 size;
};

//////////////////////////////////////////////////////////////////////////

RenderPassFinal::RenderPassFinal()
{

}

//////////////////////////////////////////////////////////////////////////

void RenderPassFinal::OnRenderCreate()
{
    Super::OnRenderCreate();

    output = MaterialLibrary::GetMaterial("assets:\\materials\\pp-image-render.xml");
}

//////////////////////////////////////////////////////////////////////////

void RenderPassFinal::OnBegin()
{
    Super::OnBegin();

    // no framebuffer bound, render the final output to the screen.

    GlCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
    GlCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

//////////////////////////////////////////////////////////////////////////

void RenderPassFinal::OnPerform()
{
    Super::OnPerform();

    Ref<RenderPassBase> bloomPass = RenderPassManager::GetPass(RenderPassBloom::Id);
    Ref<Mesh> screen = MeshLibrary::GetMesh("engine:\\mesh\\screen-quad");

    if (screen != nullptr)
    {
        ImageOutput outputData;
        outputData.position = vec2(0.0f);
        outputData.size = vec2(1.0f);

        output->SetParameterBlock<ImageOutput>("ImageDataBuffer", outputData);
        output->SetParameterValue<MaterialParameterTexture2D>("uTexture", bloomPass->GetAttachment(GBuffer::CB_Albedo)->ToTexture());

        screen->Render(output, fmat4(1.0f));
    }
}

//////////////////////////////////////////////////////////////////////////

void RenderPassFinal::OnFinish()
{
    Super::OnFinish();
}

//////////////////////////////////////////////////////////////////////////
