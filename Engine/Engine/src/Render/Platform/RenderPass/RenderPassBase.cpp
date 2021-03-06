//////////////////////////////////////////////////////////////////////////
//    File        	: RenderPass.cpp
//    Created By    : Jack Spink
//    Created On 	: [23/10/2019]
//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassBase.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Core.h"
#include "Core/Object.h"
#include "Core/Application.h"
#include "Core/Thread/GameThread.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Window.h"
#include "Render/Platform/ApiManager.h"
#include "Render/Platform/Framebuffer.h"
#include "Render/Platform/Renderer.h"
#include "Render/Platform/Buffer.h"
#include "Render/Platform/VertexArray.h"
#include "Render/Platform/Texture2D.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Utility/MaterialLibrary.h"
#include "Render/Effects/LightManager.h"

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Camera/Camera.h"

//////////////////////////////////////////////////////////////////////////

#include <random>

//////////////////////////////////////////////////////////////////////////

#ifdef _OPENGL
    #include "glfw.h"
#endif

//////////////////////////////////////////////////////////////////////////

static float2 const shadowFramebufferSize = float2(1024.0f);

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(RenderFunction, RTTIObject)
    RTTI_PROPERTY(RenderFunction, bool, enabled)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

RenderPassBase::RenderPassBase()
{
}

//////////////////////////////////////////////////////////////////////////

void RenderPassBase::OnRenderCreate()
{

}

//////////////////////////////////////////////////////////////////////////

void RenderPassBase::OnBegin()
{
}

//////////////////////////////////////////////////////////////////////////

void RenderPassBase::OnPerform()
{

}

//////////////////////////////////////////////////////////////////////////

void RenderPassBase::OnFinish()
{

}

//////////////////////////////////////////////////////////////////////////
