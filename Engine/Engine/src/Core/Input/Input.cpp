//////////////////////////////////////////////////////////////////////////
//    File        	: Input.cpp
//    Created By    : Jack Spink
//    Created On 	: [24/9/2019]
//////////////////////////////////////////////////////////////////////////

#include "Core/Input/Input.h"
#include "Core/Application.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Core.h"
#include "Render/Platform/Window.h"

//////////////////////////////////////////////////////////////////////////

#ifdef _OPENGL
#include "Render/OpenGL/OpenGlWindow.h"
#include "glfw.h"
#endif

//////////////////////////////////////////////////////////////////////////

#define RECENTRE_CURSOR 0

//////////////////////////////////////////////////////////////////////////

void Input::Update()
{
    Ref<Input> input = GetInput();
    if (input != nullptr)
    {
        input->UpdateKeyboardImpl();
        input->UpdateMouseImpl();
    }
}

//////////////////////////////////////////////////////////////////////////

bool Input::IsKeyDown(const KeyboardKey::Enum key)
{
    Ref<Input> input = GetInput();
    if (input != nullptr)
    {
        return input->IsKeyDownImpl(key);
    }

    return GetApplication()->GetWindowContext()->IsKeyDown(key);
}

//////////////////////////////////////////////////////////////////////////

KeyboardState::Enum Input::GetKeyState(const KeyboardKey::Enum key)
{
    Ref<Input> input = GetInput();
    if (input != nullptr)
    {
        return input->GetKeyStateImpl(key);
    }

    return KeyboardState::Idle;
}

//////////////////////////////////////////////////////////////////////////

bool Input::IsButtonDown(const MouseButton::Enum button)
{
    if (Ref<Input> input = GetInput())
    {
        return input->IsButtonDownImpl(button);
    }

    return GetApplication()->GetWindowContext()->IsButtonDown(button);
}

//////////////////////////////////////////////////////////////////////////

float2 Input::GetMousePosition()
{
    Ref<Input> input = GetInput();
    if (input != nullptr)
    {
        return input->currentMousePos;
    }

    return float2(0.0f);
}

//////////////////////////////////////////////////////////////////////////

float2 Input::GetMouseDelta()
{
    Ref<Input> input = GetInput();
    if (input != nullptr)
    {
        return input->mouseDelta * float2(1.0f, 1.0f);
    }

    return float2(0.0f);
}

//////////////////////////////////////////////////////////////////////////

void Input::UpdateKeyboardImpl()
{
    previousKeyStates = currentKeyStates;

    Ref<Window> window = GetApplication()->GetWindowContext();
    for (u32 index = 0; index < KeyboardKey::Max; ++index)
    {
        const bool isDown = window->IsKeyDown((KeyboardKey::Enum)index);
        currentKeyStates.at(index) = isDown;
    }
}

//////////////////////////////////////////////////////////////////////////

void Input::UpdateMouseImpl()
{
    Ref<Window> window = GetApplication()->GetWindowContext();
    const float2 mpos = window->GetMousePosition();

    mouseDelta = (mpos - currentMousePos);

#if RECENTRE_CURSOR && defined(_OPENGL)
    // set cursor to centre
    currentMousePos = float2(640.0f, 360.0f);
    GlCall(glfwSetCursorPos(std::dynamic_pointer_cast<OpenGlWindow>(window)->GetContext(), 640.0f, 360.0f));
#else
    currentMousePos = mpos;
#endif

    previousButtonStates = currentButtonStates;
    for (u32 button = 0; button < MouseButton::Max; ++button)
    {
        const bool isDown = window->IsButtonDown((MouseButton::Enum)button);
        currentButtonStates.at(button) = isDown;
    }
}

//////////////////////////////////////////////////////////////////////////

bool Input::IsKeyDownImpl(const KeyboardKey::Enum key) const
{
    return currentKeyStates.at(key);
}

//////////////////////////////////////////////////////////////////////////

KeyboardState::Enum Input::GetKeyStateImpl(const KeyboardKey::Enum key) const
{
    const bool previousState = previousKeyStates.at(key);
    const bool currentState = currentKeyStates.at(key);

    if (!previousState && !currentState)
        return KeyboardState::Idle;

    if (!previousState && currentState)
        return KeyboardState::Pressed;

    if (previousState && currentState)
        return KeyboardState::Held;

    if (previousState && !currentState)
        return KeyboardState::Released;

    return KeyboardState::Idle;
}

//////////////////////////////////////////////////////////////////////////

bool Input::IsButtonDownImpl(const MouseButton::Enum button) const
{
    return currentButtonStates.at(button);
}

//////////////////////////////////////////////////////////////////////////

void InputManager::RegisterInput(const std::string& id, KeyboardKey::Enum newKey)
{
    Ref<InputManager> manager = GetInputManager();
    if (manager != nullptr)
    {
        manager->RegisterInputImpl(id, newKey);
    }
}

//////////////////////////////////////////////////////////////////////////

void InputManager::RegisterInput(const std::string& id, const std::vector<KeyboardKey::Enum>& newKeys)
{
    Ref<InputManager> manager = GetInputManager();
    if (manager != nullptr)
    {
        manager->RegisterInputImpl(id, newKeys);
    }
}

//////////////////////////////////////////////////////////////////////////

bool InputManager::IsInputDown(const std::string& id)
{
    Ref<InputManager> manager = GetInputManager();
    if (manager != nullptr)
    {
        return manager->IsInputDownImpl(id);
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////

KeyboardState::Enum InputManager::GetInputState(const std::string& id)
{
    Ref<InputManager> manager = GetInputManager();
    if (manager != nullptr)
    {
        return manager->GetInputStateImpl(id);
    }

    return KeyboardState::Idle;
}

//////////////////////////////////////////////////////////////////////////

void InputManager::RegisterInputImpl(const std::string& id, KeyboardKey::Enum newKey)
{
    registeredInputs[id].push_back(newKey);
}

//////////////////////////////////////////////////////////////////////////

void InputManager::RegisterInputImpl(const std::string& id, const std::vector<KeyboardKey::Enum>& newKeys)
{
    for (KeyboardKey::Enum key : newKeys)
        registeredInputs[id].push_back(key);
}

//////////////////////////////////////////////////////////////////////////

bool InputManager::IsInputDownImpl(const std::string& id)
{
    auto it = registeredInputs.find(id);
    if (it != registeredInputs.end())
    {
        for (KeyboardKey::Enum key : it->second)
        {
            if (Input::IsKeyDown(key))
            {
                return true;
            }
        }
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////

KeyboardState::Enum InputManager::GetInputStateImpl(const std::string& id)
{
    auto it = registeredInputs.find(id);
    if (it != registeredInputs.end())
    {
        return Input::GetKeyState(it->second.at(0));
    }

    return KeyboardState::Idle;
}

//////////////////////////////////////////////////////////////////////////
