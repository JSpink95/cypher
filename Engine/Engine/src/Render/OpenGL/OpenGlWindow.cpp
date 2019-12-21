//////////////////////////////////////////////////////////////////////////
//    File        	: OpenGlWindow.cpp
//    Created By    : Jack Spink
//    Created On 	: [3/9/2019]
//////////////////////////////////////////////////////////////////////////

#ifdef _OPENGL

//////////////////////////////////////////////////////////////////////////

#include "Render/OpenGL/OpenGlWindow.h"
#include "Core/Events/KeyEvent.h"
#include "Core/Events/MouseEvent.h"

//////////////////////////////////////////////////////////////////////////

#include "glfw.h"
#include "Core/Events/WindowEvent.h"

//////////////////////////////////////////////////////////////////////////

bool OpenGlWindow::bHasInitialisedOpenGl = false;

//////////////////////////////////////////////////////////////////////////

OpenGlWindow::OpenGlWindow(const std::string& title, const uint2& resolution)
    : context(nullptr)
{
    if (!bHasInitialisedOpenGl)
    {
        glfwInit();
    }

    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWmonitor* primaryMonitor = nullptr;
    //primaryMonitor = glfwGetPrimaryMonitor(); // enables fullscreen

    context = glfwCreateWindow(resolution.x, resolution.y, title.c_str(), primaryMonitor, nullptr);
    glfwSetWindowUserPointer(context, this);

    if (!bHasInitialisedOpenGl)
    {
        glfwMakeContextCurrent(context);

        glewExperimental = true;
        glewInit();
    }

    bHasInitialisedOpenGl = true;

    // setup event callbacks
    glfwSetKeyCallback(context, OpenGlWindow::OnKeyEventCallback);
    glfwSetMouseButtonCallback(context, OpenGlWindow::OnMouseButtonEventCallback);
    glfwSetCursorPosCallback(context, OpenGlWindow::OnMouseMovedCallback);
    glfwSetWindowSizeCallback(context, OpenGlWindow::OnWindowResizedCallback);
    glfwSetWindowCloseCallback(context, OpenGlWindow::OnWindowClosedCallback);

    // force vsync enabled
    glfwSwapInterval(1);

    // enable multi-sampling
    glEnable(GL_MULTISAMPLE);
}

//////////////////////////////////////////////////////////////////////////

OpenGlWindow::~OpenGlWindow()
{
    glfwDestroyWindow(context);
}

//////////////////////////////////////////////////////////////////////////

bool OpenGlWindow::RequestedClose() const
{
    return glfwWindowShouldClose(context) == GLFW_TRUE;
}

//////////////////////////////////////////////////////////////////////////

void OpenGlWindow::ForceFocused()
{
    glfwMakeContextCurrent(context);
}

//////////////////////////////////////////////////////////////////////////

void OpenGlWindow::SetVisibility(const bool visible)
{
    if (visible)
    {
        glfwShowWindow(context);
    }
    else
    {
        glfwHideWindow(context);
    }
}

//////////////////////////////////////////////////////////////////////////

void OpenGlWindow::SetWindowPosition(const int2& newPosition)
{
    glfwSetWindowPos(context, newPosition.x, newPosition.y);
}

//////////////////////////////////////////////////////////////////////////

void OpenGlWindow::Resize(const uint2& resolution)
{
    glfwSetWindowSize(context, resolution.x, resolution.y);
}

//////////////////////////////////////////////////////////////////////////

void OpenGlWindow::PollEvents()
{
    glfwPollEvents();
}

//////////////////////////////////////////////////////////////////////////

void OpenGlWindow::Present()
{
    glfwSwapBuffers(context);
}

//////////////////////////////////////////////////////////////////////////

void OpenGlWindow::SetEventCallback(EventCallback callback)
{
    eventCallback = callback;
}

//////////////////////////////////////////////////////////////////////////

uint2 OpenGlWindow::GetWindowSize() const
{
    s32 w, h;
    glfwGetWindowSize(context, &w, &h);

    return uint2(w, h);
}

//////////////////////////////////////////////////////////////////////////

bool OpenGlWindow::IsKeyDown(const KeyboardKey::Enum key) const
{
    static constexpr const s32 KeyToGLFW[KeyboardKey::Max] =
    {
        GLFW_KEY_SPACE,
        GLFW_KEY_APOSTROPHE,
        GLFW_KEY_COMMA,
        GLFW_KEY_MINUS,
        GLFW_KEY_PERIOD,
        GLFW_KEY_SLASH,
        GLFW_KEY_0,
        GLFW_KEY_1,
        GLFW_KEY_2,
        GLFW_KEY_3,
        GLFW_KEY_4,
        GLFW_KEY_5,
        GLFW_KEY_6,
        GLFW_KEY_7,
        GLFW_KEY_8,
        GLFW_KEY_9,
        GLFW_KEY_SEMICOLON,
        GLFW_KEY_EQUAL,
        GLFW_KEY_A,
        GLFW_KEY_B,
        GLFW_KEY_C,
        GLFW_KEY_D,
        GLFW_KEY_E,
        GLFW_KEY_F,
        GLFW_KEY_G,
        GLFW_KEY_H,
        GLFW_KEY_I,
        GLFW_KEY_J,
        GLFW_KEY_K,
        GLFW_KEY_L,
        GLFW_KEY_M,
        GLFW_KEY_N,
        GLFW_KEY_O,
        GLFW_KEY_P,
        GLFW_KEY_Q,
        GLFW_KEY_R,
        GLFW_KEY_S,
        GLFW_KEY_T,
        GLFW_KEY_U,
        GLFW_KEY_V,
        GLFW_KEY_W,
        GLFW_KEY_X,
        GLFW_KEY_Y,
        GLFW_KEY_Z,
        GLFW_KEY_LEFT_BRACKET,
        GLFW_KEY_BACKSLASH,
        GLFW_KEY_RIGHT_BRACKET,
        GLFW_KEY_GRAVE_ACCENT,
        GLFW_KEY_WORLD_1,
        GLFW_KEY_WORLD_2,
        GLFW_KEY_ESCAPE,
        GLFW_KEY_ENTER,
        GLFW_KEY_TAB,
        GLFW_KEY_BACKSPACE,
        GLFW_KEY_INSERT,
        GLFW_KEY_DELETE,
        GLFW_KEY_RIGHT,
        GLFW_KEY_LEFT,
        GLFW_KEY_DOWN,
        GLFW_KEY_UP,
        GLFW_KEY_PAGE_UP,
        GLFW_KEY_PAGE_DOWN,
        GLFW_KEY_HOME,
        GLFW_KEY_END,
        GLFW_KEY_CAPS_LOCK,
        GLFW_KEY_SCROLL_LOCK,
        GLFW_KEY_NUM_LOCK,
        GLFW_KEY_PRINT_SCREEN,
        GLFW_KEY_PAUSE,
        GLFW_KEY_F1,
        GLFW_KEY_F2,
        GLFW_KEY_F3,
        GLFW_KEY_F4,
        GLFW_KEY_F5,
        GLFW_KEY_F6,
        GLFW_KEY_F7,
        GLFW_KEY_F8,
        GLFW_KEY_F9,
        GLFW_KEY_F10,
        GLFW_KEY_F11,
        GLFW_KEY_F12,
        GLFW_KEY_F13,
        GLFW_KEY_F14,
        GLFW_KEY_F15,
        GLFW_KEY_F16,
        GLFW_KEY_F17,
        GLFW_KEY_F18,
        GLFW_KEY_F19,
        GLFW_KEY_F20,
        GLFW_KEY_F21,
        GLFW_KEY_F22,
        GLFW_KEY_F23,
        GLFW_KEY_F24,
        GLFW_KEY_F25,
        GLFW_KEY_KP_0,
        GLFW_KEY_KP_1,
        GLFW_KEY_KP_2,
        GLFW_KEY_KP_3,
        GLFW_KEY_KP_4,
        GLFW_KEY_KP_5,
        GLFW_KEY_KP_6,
        GLFW_KEY_KP_7,
        GLFW_KEY_KP_8,
        GLFW_KEY_KP_9,
        GLFW_KEY_KP_DECIMAL,
        GLFW_KEY_KP_DIVIDE,
        GLFW_KEY_KP_MULTIPLY,
        GLFW_KEY_KP_SUBTRACT,
        GLFW_KEY_KP_ADD,
        GLFW_KEY_KP_ENTER,
        GLFW_KEY_KP_EQUAL,
        GLFW_KEY_LEFT_SHIFT,
        GLFW_KEY_LEFT_CONTROL,
        GLFW_KEY_LEFT_ALT,
        GLFW_KEY_LEFT_SUPER,
        GLFW_KEY_RIGHT_SHIFT,
        GLFW_KEY_RIGHT_CONTROL,
        GLFW_KEY_RIGHT_ALT,
        GLFW_KEY_RIGHT_SUPER,
        GLFW_KEY_MENU,
    };

    return glfwGetKey(context, KeyToGLFW[key]) == GLFW_PRESS;
}

//////////////////////////////////////////////////////////////////////////

float2 OpenGlWindow::GetMousePosition() const
{
    f64 mx, my;
    glfwGetCursorPos(context, &mx, &my);

    return float2((f32)mx, (f32)my);
}

//////////////////////////////////////////////////////////////////////////

void OpenGlWindow::OnKeyEventCallback(GLFWwindow* context, s32 key, s32 scancode, s32 action, s32 modifiers)
{
    OpenGlWindow& window = *(OpenGlWindow*)glfwGetWindowUserPointer(context);

    switch (action)
    {
        case GLFW_PRESS:
        {
            KeyPressedEvent event(key, false);
            window.eventCallback(event);
            break;
        }

        case GLFW_RELEASE:
        {
            KeyReleasedEvent event(key);
            window.eventCallback(event);
            break;
        }

        case GLFW_REPEAT:
        {
            KeyPressedEvent event(key, true);
            window.eventCallback(event);
            break;
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void OpenGlWindow::OnMouseButtonEventCallback(GLFWwindow* context, s32 button, s32 action, s32 modifiers)
{
    OpenGlWindow& window = *(OpenGlWindow*)glfwGetWindowUserPointer(context);

    switch (action)
    {
        case GLFW_PRESS:
        {
            MouseButtonPressedEvent event(button);
            window.eventCallback(event);
            break;
        }

        case GLFW_RELEASE:
        {
            MouseButtonReleasedEvent event(button);
            window.eventCallback(event);
            break;
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void OpenGlWindow::OnMouseMovedCallback(GLFWwindow* context, f64 x, f64 y)
{
    OpenGlWindow& window = *(OpenGlWindow*)glfwGetWindowUserPointer(context);

    MouseMovedEvent event((f32)x, (f32)y);
    window.eventCallback(event);
}

//////////////////////////////////////////////////////////////////////////

void OpenGlWindow::OnMouseWheelScrolledCallback(GLFWwindow* context, f64 xscroll, f64 yscroll)
{
    OpenGlWindow& window = *(OpenGlWindow*)glfwGetWindowUserPointer(context);

    MouseWheelScrolledEvent event((f32)xscroll, (f32)yscroll);
    window.eventCallback(event);
}

//////////////////////////////////////////////////////////////////////////

void OpenGlWindow::OnWindowResizedCallback(GLFWwindow* context, s32 width, s32 height)
{
    OpenGlWindow& window = *(OpenGlWindow*)glfwGetWindowUserPointer(context);

    WindowResizeEvent event(uint2((u32)width, (u32)height));
    window.eventCallback(event);
}

//////////////////////////////////////////////////////////////////////////

void OpenGlWindow::OnWindowClosedCallback(GLFWwindow* context)
{
    OpenGlWindow& window = *(OpenGlWindow*)glfwGetWindowUserPointer(context);

    WindowCloseEvent event;
    window.eventCallback(event);
}

//////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////
