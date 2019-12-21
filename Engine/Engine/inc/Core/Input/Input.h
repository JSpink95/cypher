//////////////////////////////////////////////////////////////////////////
//    File        	: Input.h
//    Created By    : Jack Spink
//    Created On 	: [24/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"
#include "Core/Singleton.h"

//////////////////////////////////////////////////////////////////////////

#include <array>
#include <vector>
#include <unordered_map>

//////////////////////////////////////////////////////////////////////////

namespace KeyboardKey
{
    enum Enum
    {
        /* normal keys*/
        SPACE,
        APOSTROPHE,
        COMMA,
        MINUS,
        PERIOD,
        SLASH,
        N0,
        N1,
        N2,
        N3,
        N4,
        N5,
        N6,
        N7,
        N8,
        N9,
        SEMICOLON,
        EQUAL,
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        LEFT_BRACKET,
        BACKSLASH,
        RIGHT_BRACKET,
        GRAVE_ACCENT,
        WORLD_1,
        WORLD_2,

        /* Function keys */
        ESCAPE,
        ENTER,
        TAB,
        BACKSPACE,
        INSERT,
        DEL,
        RIGHT,
        LEFT,
        DOWN,
        UP,
        PAGE_UP,
        PAGE_DOWN,
        HOME,
        END,
        CAPS_LOCK,
        SCROLL_LOCK,
        NUM_LOCK,
        PRINT_SCREEN,
        PAUSE,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        F13,
        F14,
        F15,
        F16,
        F17,
        F18,
        F19,
        F20,
        F21,
        F22,
        F23,
        F24,
        F25,
        KP_0,
        KP_1,
        KP_2,
        KP_3,
        KP_4,
        KP_5,
        KP_6,
        KP_7,
        KP_8,
        KP_9,
        KP_DECIMAL,
        KP_DIVIDE,
        KP_MULTIPLY,
        KP_SUBTRACT,
        KP_ADD,
        KP_ENTER,
        KP_EQUAL,
        LEFT_SHIFT,
        LEFT_CONTROL,
        LEFT_ALT,
        LEFT_SUPER,
        RIGHT_SHIFT,
        RIGHT_CONTROL,
        RIGHT_ALT,
        RIGHT_SUPER,
        MENU,
        Max,
    };
}

//////////////////////////////////////////////////////////////////////////

namespace KeyboardState
{
    enum Enum
    {
        Idle, Pressed, Held, Released, Max
    };
}

//////////////////////////////////////////////////////////////////////////

class Input: public Singleton<Input>
{
public:
    static void Update();

    // 
    // keyboard
    // 

    static bool IsKeyDown(const KeyboardKey::Enum key);
    static KeyboardState::Enum GetKeyState(const KeyboardKey::Enum key);

    // 
    // mouse
    // 

    static float2 GetMousePosition();
    static float2 GetMouseDelta();

private:
    void UpdateKeyboardImpl();
    void UpdateMouseImpl();

    bool IsKeyDownImpl(const KeyboardKey::Enum key) const;
    KeyboardState::Enum GetKeyStateImpl(const KeyboardKey::Enum key) const;

private:

    // 
    // keyboard
    // 

    std::array<bool, KeyboardKey::Max> previousKeyStates;
    std::array<bool, KeyboardKey::Max> currentKeyStates;

    // 
    // cursor
    // 

    float2 currentMousePos;
    float2 mouseDelta;
};

//////////////////////////////////////////////////////////////////////////

static inline Ref<Input> GetInput() { return Input::Get(); }

//////////////////////////////////////////////////////////////////////////

class InputManager : public Singleton<InputManager>
{
public:
    static void RegisterInput(const std::string& id, KeyboardKey::Enum newKey);
    static void RegisterInput(const std::string& id, const std::vector<KeyboardKey::Enum>& newKeys);

    static bool IsInputDown(const std::string& id);
    static KeyboardState::Enum GetInputState(const std::string& id);

private:
    void RegisterInputImpl(const std::string& id, KeyboardKey::Enum newKey);
    void RegisterInputImpl(const std::string& id, const std::vector<KeyboardKey::Enum>& newKeys);
    bool IsInputDownImpl(const std::string& id);
    KeyboardState::Enum GetInputStateImpl(const std::string& id);

private:
    std::unordered_map<std::string, std::vector<KeyboardKey::Enum>> registeredInputs;
};

//////////////////////////////////////////////////////////////////////////

static inline Ref<InputManager> GetInputManager() { return InputManager::Get(); }

//////////////////////////////////////////////////////////////////////////
