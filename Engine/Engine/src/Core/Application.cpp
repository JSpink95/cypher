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
#include "Core/Utility/FileVolumeManager.h"

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

#ifdef _OPENGL

#include "glfw.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Render/OpenGL/OpenGlWindow.h"

#endif // _OPENGL

//////////////////////////////////////////////////////////////////////////

Application::Application()
    : windowClosed(false)
    , time(0.0f)
    , deltaTime(0.0f)
{
    instance = this;
    global_random::initialise();

    FileVolumeManager::Create();
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
    FileVolumeManager::Delete();
}

//////////////////////////////////////////////////////////////////////////

void Application::Create()
{
    // 
    // initialise threads
    // 

    GameThread::Create();
    PhysicsThread::Create();

    OnCreate();

    mainThreadInitialised = true;
}

//////////////////////////////////////////////////////////////////////////

void Application::RenderCreate()
{
    // create the window
    //window = GetApiManager()->CreateContext("Application", DefaultWindowSize);
    window = GetApiManager()->CreateContext("Application", uint2(1280, 720));
    window->SetEventCallback(BIND_FUNCTION_OneParam(Application::OnEvent));
    window->SetVisibility(true);

    TextureLibrary::Initialise();
    MaterialLibrary::Initialise();
    MeshLibrary::Initialise();
    RenderPassManager::Initialise();

    ImGui::CreateContext();
    ImGui::StyleColorsDark();

#ifdef _OPENGL
    OpenGlWindow* glWindow = static_cast<OpenGlWindow*>(window.get());
    ImGui_ImplGlfw_InitForOpenGL(glWindow->GetContext(), true);
    ImGui_ImplOpenGL3_Init("#version 130");
#elif defined(_DX11)
    Dx11Window* dxWindow = static_cast<Dx11Window*>(window.get());
    // ImGui_ImplDX11();
#endif

    GlCall(glEnable(GL_DEPTH_TEST));
    GlCall(glEnable(GL_CULL_FACE));

    OnRenderCreate();

    renderThreadInitialised = true;
}

//////////////////////////////////////////////////////////////////////////

void Application::PostCreate()
{
    OnPostCreate();
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
}

//////////////////////////////////////////////////////////////////////////

void Application::OnImGuiRender()
{
    // there be dragons here
}

//////////////////////////////////////////////////////////////////////////

void Application::OnPresent()
{
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
