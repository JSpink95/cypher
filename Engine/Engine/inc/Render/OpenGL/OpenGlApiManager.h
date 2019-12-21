//////////////////////////////////////////////////////////////////////////
//    File        	: OpenGlApiManager.h
//    Created By    : Jack Spink
//    Created On 	: [3/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/ApiManager.h"

//////////////////////////////////////////////////////////////////////////

class OpenGlApiManager : public ApiManager
{
public:
    OpenGlApiManager();
    virtual ~OpenGlApiManager();

    // 
    // window creation
    // 

    virtual Ref<Window> CreateContext(const std::string& title, const uint2& resolution) override;

    // 
    // shader creation
    // 

    virtual Ref<Shader> CreateShader(const std::string& shaderpath) override;
    virtual Ref<Shader> CreateShader(const pugi::xml_node& shaderNode) override;
    virtual Ref<ComputeShader> CreateComputeShader(const std::string& shaderpath) override;

    // 
    // texture creation
    // 

    virtual Ref<Texture2D> CreateTexture2D(const std::string& texturepath) override;
    virtual Ref<Texture2D> CreateTexture2D(const uint2& resolution, const float3* rgb) override;
    virtual Ref<Texture2D> CreateTexture2D(const uint2& resolution, const float4* rgba) override;

    // 
    // framebuffer creation
    // 

    virtual Ref<Framebuffer> CreateFramebuffer(const FramebufferData& initData) override;

    // 
    // vertex buffer/array creation
    // 

    virtual Ref<VertexBuffer> CreateVertexBuffer(const size_t count, const size_t stride, const void* vertices) override;
    virtual Ref<VertexArray> CreateVertexArray() override;

};

//////////////////////////////////////////////////////////////////////////

static inline Ref<OpenGlApiManager> GetOpenGlApiManager() { return RefCast<OpenGlApiManager, ApiManager>(ApiManager::GetInstance()); }

//////////////////////////////////////////////////////////////////////////
