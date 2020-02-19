//////////////////////////////////////////////////////////////////////////
//    File        	: Application.h
//    Created By    : Jack Spink
//    Created On 	: [3/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include <memory>
#include <future>

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"
#include "Events/EventDispatcher.h"
#include "Events/MouseEvent.h"

//////////////////////////////////////////////////////////////////////////

class Window;
class Event;
class WindowCloseEvent;
class WindowResizeEvent;

//////////////////////////////////////////////////////////////////////////

class Application
{
public:
    using WindowResizeListener = std::function<void(const uint2& newWindowSize)>;

public:
    Application();
    virtual ~Application();

    void Create();
    void RenderCreate();
    void Destroy();

    void PostCreate();

    // called from the GameThread
    virtual void OnPreUpdate(const f32 dt);
    virtual void OnPostUpdate();

    // called from the PhysicsThread
    virtual void OnPrePhysics();
    virtual void OnPostPhysics();

    // called from the RenderThread
    virtual void OnPreRender();
    virtual void OnPostRender();
    virtual void OnImGuiRender();
    virtual void OnPresent();

    // called from the MainThread
    virtual void OnAppTick();

    virtual bool RequestedShutdown() const { return windowClosed; }
    virtual inline Ref<Window> GetWindowContext() const { return window; }

    bool IsInitialised() const { return mainThreadInitialised && renderThreadInitialised; }

protected:

    virtual void OnCreate() {}
    virtual void OnRenderCreate() {}
    virtual void OnPostCreate() {}

    virtual void OnDestroy() {}

    virtual void OnEvent(Event& evt);
    virtual void OnWindowClosed(WindowCloseEvent& closeEvent);
    virtual void OnWindowResized(WindowResizeEvent& resizeEvent);
    virtual void OnMouseMoved(MouseMovedEvent& mouseMovedEvent);

    void SubscribeWindowResizeListener(WindowResizeListener listener);

protected:
    Ref<Window> window;

private:
    f32 time, deltaTime;
    bool windowClosed;

    bool mainThreadInitialised = false;
    bool renderThreadInitialised = false;

private:
    std::vector<WindowResizeListener> windowResizeListeners;

public:
    static inline Application* GetInstance() { return instance; }

private:
    static inline Application* instance = nullptr;

public:
    template<typename TApplication>
    static void Main();
};

//////////////////////////////////////////////////////////////////////////

static Application* GetApplication() { return Application::GetInstance(); }

//////////////////////////////////////////////////////////////////////////

template<typename TApplication>
void Application::Main()
{
    TApplication application;
    application.Create();

    // wait until the render thread is initialised
    while (!application.IsInitialised());

    application.PostCreate();

    while (!application.RequestedShutdown())
    {
        application.OnAppTick();

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    application.Destroy();
}

//////////////////////////////////////////////////////////////////////////
