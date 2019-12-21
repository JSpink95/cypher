//////////////////////////////////////////////////////////////////////////
//    File        	: OpenGlApiManager.cpp
//    Created By    : Jack Spink
//    Created On 	: [3/9/2019]
//////////////////////////////////////////////////////////////////////////

#ifdef _OPENGL

//////////////////////////////////////////////////////////////////////////

#include "glfw.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Core.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/OpenGL/OpenGlApiManager.h"
#include "Render/OpenGL/OpenGlWindow.h"
#include "Render/OpenGL/OpenGlShader.h"
#include "Render/OpenGL/OpenGlTexture2D.h"
#include "Render/OpenGL/OpenGlBuffer.h"
#include "Render/OpenGL/OpenGlVertexArray.h"
#include "Render/OpenGL/OpenGlFramebuffer.h"
#include "Render/OpenGL/OpenGlComputeShader.h"

//////////////////////////////////////////////////////////////////////////

OpenGlApiManager::OpenGlApiManager()
{
}

//////////////////////////////////////////////////////////////////////////

OpenGlApiManager::~OpenGlApiManager()
{
    glfwTerminate();
}

//////////////////////////////////////////////////////////////////////////


Ref<Window> OpenGlApiManager::CreateContext(const std::string& title, const uint2& resolution)
{
    return std::make_shared<OpenGlWindow>(title, resolution);
}

//////////////////////////////////////////////////////////////////////////

Ref<Shader> OpenGlApiManager::CreateShader(const std::string& shaderpath)
{
    return std::make_shared<OpenGlShader>(shaderpath);
}

Ref<Shader> OpenGlApiManager::CreateShader(const pugi::xml_node& shaderNode)
{
    return std::make_shared<OpenGlShader>(shaderNode);
}

Ref<ComputeShader> OpenGlApiManager::CreateComputeShader(const std::string& source)
{
    return std::make_shared<OpenGlComputeShader>(source);
}

//////////////////////////////////////////////////////////////////////////

Ref<Texture2D> OpenGlApiManager::CreateTexture2D(const std::string& texturepath)
{
    return std::make_shared<OpenGlTexture2D>(texturepath);
}

Ref<Texture2D> OpenGlApiManager::CreateTexture2D(const uint2& resolution, const float3* rgb)
{
	return std::make_shared<OpenGlTexture2D>(resolution, rgb);
}

Ref<Texture2D> OpenGlApiManager::CreateTexture2D(const uint2& resolution, const float4* rgba)
{
    return std::make_shared<OpenGlTexture2D>(resolution, rgba);
}

//////////////////////////////////////////////////////////////////////////

Ref<Framebuffer> OpenGlApiManager::CreateFramebuffer(const FramebufferData& initData)
{
    return std::make_shared<OpenGlFramebuffer>(initData);
}

//////////////////////////////////////////////////////////////////////////

Ref<VertexBuffer> OpenGlApiManager::CreateVertexBuffer(const size_t count, const size_t stride, const void* vertices)
{
    return std::make_shared<OpenGlVertexBuffer>(count, stride, vertices);
}

//////////////////////////////////////////////////////////////////////////

Ref<VertexArray> OpenGlApiManager::CreateVertexArray()
{
    return std::make_shared<OpenGlVertexArray>();
}

//////////////////////////////////////////////////////////////////////////

#endif // _OPENGL

//////////////////////////////////////////////////////////////////////////
