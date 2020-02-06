//////////////////////////////////////////////////////////////////////////
//    File        	: RenderPassSSAO.h
//    Created By    : Jack Spink
//    Created On 	: [27/01/2020]
//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassSSAO.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Core.h"
#include "Core/Utility/RandomUtils.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Mesh.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Utility/MeshLibrary.h"
#include "Render/Utility/MaterialLibrary.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Renderer.h"
#include "Render/Platform/ApiManager.h"
#include "Render/Platform/Framebuffer.h"
#include "Render/Platform/Texture2D.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassManager.h"
#include "Render/Platform/RenderPass/RenderPassLit.h"

//////////////////////////////////////////////////////////////////////////

#ifdef _OPENGL
    #include "glfw.h"
#endif

//////////////////////////////////////////////////////////////////////////

struct SsaoInput
{
    static inline constexpr size_t MaxKernelSize = 32;

    vec4 kernel[MaxKernelSize] = { vec4(0.0f) };
    vec2 noiseScale = vec2(1.0f);
};

//////////////////////////////////////////////////////////////////////////

RenderPassSSAO::RenderPassSSAO()
{

}

//////////////////////////////////////////////////////////////////////////

Ref<FramebufferAttachment> RenderPassSSAO::GetAttachment(GBuffer::Color attachment)
{
    return framebuffer->GetColorBuffer(attachment);
}

//////////////////////////////////////////////////////////////////////////

void RenderPassSSAO::OnRenderCreate()
{
    Super::OnRenderCreate();

    FramebufferData fb;
    fb.resolution = (uint2)(float2(1280.0f, 720.0f) / 4.0f);
    fb.colorBuffers.at(GBuffer::CB_Albedo) = { true, false };

    framebuffer = GetApiManager()->CreateFramebuffer(fb);

    float3 noisePixels[4 * 4];
    for (u32 index = 0; index < 4 * 4; ++index)
    {
        noisePixels[index].x = global_random::as_float(-1.0f, 1.0f);
        noisePixels[index].y = global_random::as_float(-1.0f, 1.0f);
        noisePixels[index].z = 0.0f;
    }

    Ref<Texture2D> noiseTexture = GetApiManager()->CreateTexture2D(uint2(4, 4), noisePixels);
    noiseTexture->SetTextureParameter(TextureParameterType::WrapMode, TextureParameterValue::Repeat);
    
    Ref<Texture2D> positionTexture = nullptr;
    Ref<Texture2D> normalTexture = nullptr;

    if (Ref<RenderPassBase> litPass = RenderPassManager::GetPass(RenderPassLit::Id))
    {
        positionTexture = litPass->GetAttachment(GBuffer::CB_Position)->ToTexture();
        normalTexture = litPass->GetAttachment(GBuffer::CB_Normal)->ToTexture();
    }

    SsaoInput ssaoInput;
    ssaoInput.noiseScale = (float2(1280.0f, 720.0f) / float2(4.0f, 4.0f)) / 4.0f;   // add extra scale down to account for framebuffer scaling

    for (u32 index = 0; index < SsaoInput::MaxKernelSize; ++index)
    {
        vec3 sample(
            global_random::as_float(-1.0f, 1.0f),
            global_random::as_float(-1.0f, 1.0f),
            global_random::as_float( 0.0f, 1.0f)
        );

        sample = glm::normalize(sample);
        sample *= global_random::as_float(0.0f, 1.0f);

        f32 scale = (f32)index / 32.0f;
        scale = glm::mix(0.1f, 1.0f, scale * scale);
        sample *= scale;

        ssaoInput.kernel[index] = vec4(sample, 0.0f);
    }

    ssao = MaterialLibrary::GetMaterial("assets:\\materials\\pp-ssao.xml")->Clone();
    if (ssao != nullptr)
    {
        ssao->SetParameterBlock<SsaoInput>("SsaoInput", ssaoInput);
        ssao->SetParameterValue<MaterialParameterTexture2D>("uPosition", positionTexture);
        ssao->SetParameterValue<MaterialParameterTexture2D>("uNormal", normalTexture);
        ssao->SetParameterValue<MaterialParameterTexture2D>("uNoise", noiseTexture);
    }
}

//////////////////////////////////////////////////////////////////////////

void RenderPassSSAO::OnBegin()
{
    Super::OnBegin();

    Renderer::BeginScene(framebuffer);
    
    GlCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
    GlCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

//////////////////////////////////////////////////////////////////////////

void RenderPassSSAO::OnPerform()
{
    Super::OnPerform();

    if (Ref<Mesh> screen = MeshLibrary::GetMesh("engine:\\mesh\\screen-quad"))
    {
        screen->Render(ssao, fmat4(1.0f));
    }
}

//////////////////////////////////////////////////////////////////////////

void RenderPassSSAO::OnFinish()
{
    Super::OnFinish();

    Renderer::EndScene();
}

//////////////////////////////////////////////////////////////////////////
