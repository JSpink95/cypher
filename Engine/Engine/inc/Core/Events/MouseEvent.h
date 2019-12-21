//////////////////////////////////////////////////////////////////////////
//    File        	: MouseEvent.h
//    Created By    : Jack Spink
//    Created On 	: [3/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Events/Event.h"

//////////////////////////////////////////////////////////////////////////

class MouseMovedEvent : public Event
{
public:
    MouseMovedEvent(const f32 x, const f32 y);

    inline const f32 GetX() const { return x; }
    inline const f32 GetY() const { return y; }

public:
    EVENT_CATEGORY_TYPE(EventCategory::Mouse);
    EVENT_CLASS_TYPE(MouseMoved)

private:
    f32 x;
    f32 y;
};

//////////////////////////////////////////////////////////////////////////

class MouseWheelScrolledEvent : public Event
{
public:
    MouseWheelScrolledEvent(const f32 xscroll, const f32 yscroll);

    inline const f32 GetScrollX() const { return xscroll; }
    inline const f32 GetScrollY() const { return yscroll; }

public:
    EVENT_CATEGORY_TYPE(EventCategory::Mouse);
    EVENT_CLASS_TYPE(MouseWheelScrolled)

private:
    f32 xscroll;
    f32 yscroll;
};

//////////////////////////////////////////////////////////////////////////

class MouseButtonEvent : public Event
{
public:
    MouseButtonEvent(const s32 button);

    inline s32 GetButton() const { return button; }

    EVENT_CATEGORY_TYPE(EventCategory::Mouse | EventCategory::Input);

private:
    s32 button;
};

//////////////////////////////////////////////////////////////////////////

class MouseButtonPressedEvent : public MouseButtonEvent
{
public:
    MouseButtonPressedEvent(const s32 button);

    EVENT_CLASS_TYPE(MouseButtonPressed)
};

//////////////////////////////////////////////////////////////////////////

class MouseButtonReleasedEvent : public MouseButtonEvent
{
public:
    MouseButtonReleasedEvent(const s32 button);

    EVENT_CLASS_TYPE(MouseButtonReleased)
};

//////////////////////////////////////////////////////////////////////////
