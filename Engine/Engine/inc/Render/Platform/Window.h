//////////////////////////////////////////////////////////////////////////
//    File        	: Window.h
//    Created By    : Jack Spink
//    Created On 	: [3/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"
#include "Core/Input/Input.h"

//////////////////////////////////////////////////////////////////////////

#include <functional>

//////////////////////////////////////////////////////////////////////////

class Event;

//////////////////////////////////////////////////////////////////////////

using EventCallback = std::function<void(Event&)>;

//////////////////////////////////////////////////////////////////////////

class Window
{
public:
    virtual ~Window() = default;

public:
    virtual bool RequestedClose() const = 0;
    virtual void ForceFocused() = 0;
    virtual void SetVisibility(const bool visible) = 0;
    virtual void SetWindowPosition(const int2& newPosition) = 0;
    virtual void Resize(const uint2& resolution) = 0;

    virtual void Recentre() = 0;

    virtual void PollEvents() = 0;
    virtual void Present() = 0;

    virtual void SetEventCallback(EventCallback eventCallback) = 0;

    virtual uint2 GetWindowSize() const = 0;

public:

    // input queries
    virtual bool IsKeyDown(const KeyboardKey::Enum key) const = 0;
    virtual float2 GetMousePosition() const = 0;
};

//////////////////////////////////////////////////////////////////////////
