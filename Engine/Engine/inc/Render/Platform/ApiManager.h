//////////////////////////////////////////////////////////////////////////
//    File        	: ApiManager.h
//    Created By    : Jack Spink
//    Created On 	: [2/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"

//////////////////////////////////////////////////////////////////////////

#include <string>

//////////////////////////////////////////////////////////////////////////

#include "pugixml.hpp"
#include "Render/Platform/Framebuffer.h"

//////////////////////////////////////////////////////////////////////////

// 
// forward declarations
// 

class Window;
class Shader;
class ComputeShader;
class Texture2D;
class VertexBuffer;
class VertexArray;

//////////////////////////////////////////////////////////////////////////

class ApiManager
{
protected:
    ApiManager() = default;
    virtual ~ApiManager() = default;

public:

    // 
    // window creation
    // 

    virtual Ref<Window> CreateContext(const std::string& title, const uint2& resolution) = 0;

    // 
    // shader creation
    // 

    virtual Ref<Shader> CreateShader(const std::string& shaderpath) = 0;
    virtual Ref<Shader> CreateShader(const pugi::xml_node& shaderNode) = 0;
    virtual Ref<ComputeShader> CreateComputeShader(const std::string& shaderpath) = 0;

    // 
    // texture creation
    // 

	virtual Ref<Texture2D> CreateTexture2D(const std::string& texturepath) = 0;
    virtual Ref<Texture2D> CreateTexture2D(const uint2& resolution, const float3* rgb) = 0;
    virtual Ref<Texture2D> CreateTexture2D(const uint2& resolution, const float4* rgba) = 0;

    // 
    // framebuffer creation
    // 

    virtual Ref<Framebuffer> CreateFramebuffer(const FramebufferData& initData) = 0;

    // 
    // vertex buffer/array creation
    // 

    virtual Ref<VertexBuffer> CreateVertexBuffer(const size_t count, const size_t stride, const void* vertices) = 0;
    virtual Ref<VertexArray> CreateVertexArray() = 0;

public:

    static inline Ref<ApiManager> GetInstance() { return instance; }

    static void Create();
    static void Delete();

private:

    static inline Ref<ApiManager> instance = nullptr;

};

//////////////////////////////////////////////////////////////////////////

static inline Ref<ApiManager> GetApiManager() { return ApiManager::GetInstance(); }

//////////////////////////////////////////////////////////////////////////
