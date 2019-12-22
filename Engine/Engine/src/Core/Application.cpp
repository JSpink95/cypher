//////////////////////////////////////////////////////////////////////////
//    File        	: Application.cpp
//    Created By    : Jack Spink
//    Created On 	: [3/9/2019]
//////////////////////////////////////////////////////////////////////////

#include "Core/Core.h"
#include "Core/Application.h"
#include "Core/ConsoleLogger.h"
#include "Core/ObjectManager.h"
#include "Core/Events/Event.h"
#include "Core/Events/WindowEvent.h"
#include "Core/Utility/RandomUtils.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Thread/GameThread.h"
#include "Core/Thread/PhysicsThread.h"
#include "Core/Thread/RenderThread.h"
#include "Core/Input/Input.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Window.h"
#include "Render/Platform/ApiManager.h"
#include "Render/Platform/Renderer.h"
#include "Render/Platform/Texture2D.h"
#include "Render/Platform/RenderPass.h"
#include "Render/Effects/LightManager.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Utility/TextureLibrary.h"
#include "Render/Utility/MaterialLibrary.h"
#include "Render/Utility/MeshLibrary.h"

//////////////////////////////////////////////////////////////////////////

#include "glfw.h"

//////////////////////////////////////////////////////////////////////////

Application::Application()
    : windowClosed(false)
    , time(0.0f)
    , deltaTime(0.0f)
{
    instance = this;

    ConsoleLogger::Init();
    Random::Create();
    Input::Create();
    InputManager::Create();

    ApiManager::Create();
    TextureLibrary::Create();
    MaterialLibrary::Create();
    MeshLibrary::Create();
    RenderPassManager::Create();
    LightManager::Create();
    ObjectManager::Create();
}

//////////////////////////////////////////////////////////////////////////

Application::~Application()
{
    ObjectManager::Delete();
    LightManager::Delete();
    RenderPassManager::Delete();
    MeshLibrary::Delete();
    MaterialLibrary::Delete();
    TextureLibrary::Delete();
    ApiManager::Delete();
    InputManager::Delete();
    Input::Delete();
    Random::Delete();
}

//////////////////////////////////////////////////////////////////////////

void Application::Create()
{
    // 
    // initialise threads
    // 

    GameThread::Create();
    PhysicsThread::Create();

    // allow the application to initialise all render data
    GetGameThread()->PushThreadTask(this, &Application::RenderTask_OnCreate);

    OnCreate();
}

//////////////////////////////////////////////////////////////////////////

void Application::Destroy()
{
    OnDestroy();
    window = nullptr;

    GameThread::Delete();
    PhysicsThread::Delete();
}

//////////////////////////////////////////////////////////////////////////

void Application::OnPreUpdate(const f32 dt)
{
    time += dt;
    deltaTime = dt;

    Input::Update();
}

//////////////////////////////////////////////////////////////////////////

void Application::OnPostUpdate()
{

}

//////////////////////////////////////////////////////////////////////////

void Application::OnPrePhysics()
{

}

//////////////////////////////////////////////////////////////////////////

void Application::OnPostPhysics()
{

}

//////////////////////////////////////////////////////////////////////////

void Application::OnPreRender()
{
    Renderer::SetTimeAndDeltaTime(time, deltaTime);
}

//////////////////////////////////////////////////////////////////////////

void Application::OnPostRender()
{
    Renderer::EndScene();

    window->PollEvents();
    window->Present();
}

//////////////////////////////////////////////////////////////////////////

void Application::OnAppTick()
{
    
}

//////////////////////////////////////////////////////////////////////////

void Application::OnEvent(Event& event)
{
    SystemEventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowCloseEvent>(BIND_FUNCTION_OneParam(Application::OnWindowClosed));
    //dispatcher.Dispatch<MouseMovedEvent>(BIND_FUNCTION_OneParam(Application::OnMouseMoved));
}

//////////////////////////////////////////////////////////////////////////

void Application::OnWindowClosed(WindowCloseEvent& closeEvent)
{
    windowClosed = true;
}

//////////////////////////////////////////////////////////////////////////

void Application::OnMouseMoved(MouseMovedEvent& mouseMovedEvent)
{
}

//////////////////////////////////////////////////////////////////////////

void Application::RenderTask_OnCreate()
{
    // create the window
    //window = GetApiManager()->CreateContext("Application", DefaultWindowSize);
    window = GetApiManager()->CreateContext("Application", uint2(1920, 1080));
    window->SetEventCallback(BIND_FUNCTION_OneParam(Application::OnEvent));
    window->SetVisibility(true);

    TextureLibrary::Initialise();
    MaterialLibrary::Initialise();
    MeshLibrary::Initialise();
    RenderPassManager::Initialise();

    GlCall(glEnable(GL_DEPTH_TEST));
    GlCall(glEnable(GL_CULL_FACE));
}

//////////////////////////////////////////////////////////////////////////
