//////////////////////////////////////////////////////////////////////////
//    File        	: OpenGlWindow.h
//    Created By    : Jack Spink
//    Created On 	: [3/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Window.h"

//////////////////////////////////////////////////////////////////////////

#include <string>

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"

//////////////////////////////////////////////////////////////////////////

// 
// forward declarations
// 

struct GLFWwindow;

//////////////////////////////////////////////////////////////////////////

class OpenGlWindow : public Window
{
public:
    OpenGlWindow(const std::string& title, const uint2& resolution);
    virtual ~OpenGlWindow();

    // 
    // overridden interface functions
    // 

    virtual bool RequestedClose() const override;

    virtual void ForceFocused() override;
    virtual void SetVisibility(const bool visible) override;
    virtual void SetWindowPosition(const int2& newPosition) override;
    virtual void Resize(const uint2& resolution) override;

    virtual void Recentre() override;

    virtual void PollEvents() override;
    virtual void Present() override;

    virtual void SetEventCallback(EventCallback eventCallback) override;

    virtual uint2 GetWindowSize() const override;

    // 
    // input
    // 

    virtual bool IsKeyDown(const KeyboardKey::Enum key) const override;
    virtual float2 GetMousePosition() const override;

    // 
    // opengl specific
    // 

    inline GLFWwindow* GetContext() { return context; }

private:
    GLFWwindow* context;
    EventCallback eventCallback;

private:
    static bool bHasInitialisedOpenGl;

    static void OnKeyEventCallback(GLFWwindow* context, s32 key, s32 scancode, s32 action, s32 modifiers);
    static void OnMouseButtonEventCallback(GLFWwindow* context, s32 button, s32 action, s32 modifiers);
    static void OnMouseMovedCallback(GLFWwindow* context, f64 x, f64 y);
    static void OnMouseWheelScrolledCallback(GLFWwindow* context, f64 xscroll, f64 yscroll);
    static void OnWindowResizedCallback(GLFWwindow* context, s32 width, s32 height);
    static void OnWindowClosedCallback(GLFWwindow* context);
};

//////////////////////////////////////////////////////////////////////////
