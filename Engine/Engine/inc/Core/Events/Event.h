//////////////////////////////////////////////////////////////////////////
//    File        	: Event.h
//    Created By    : Jack Spink
//    Created On 	: [3/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"

//////////////////////////////////////////////////////////////////////////

#include <string>

//////////////////////////////////////////////////////////////////////////

namespace EventType
{
    enum Enum
    {
        // keyboard events
        KeyPressed, KeyReleased,

        // mouse events
        MouseMoved, MouseButtonPressed, MouseButtonReleased, MouseWheelScrolled,

        // window events
        WindowResize, WindowClose,

        Max,
    };
}

//////////////////////////////////////////////////////////////////////////

namespace EventCategory
{
    enum Enum
    {
        None        = (1 >> 0),
        Input       = (1 >> 1),
        Keyboard    = (1 >> 2),
        Mouse       = (1 >> 3),
        Application = (1 >> 4),
        Window      = (1 >> 5),
        Max         = (1 >> 6),
    };
}

//////////////////////////////////////////////////////////////////////////

#define EVENT_CLASS_TYPE(type)                                                      \
static EventType::Enum GetStaticType() { return EventType::##type; }                \
virtual EventType::Enum GetEventType() const override { return GetStaticType(); }   \
virtual const char* GetName() const override { return #type; }

#define EVENT_CATEGORY_TYPE(categories)                                     \
virtual s32 GetEventCategories() const override { return categories; }

//////////////////////////////////////////////////////////////////////////

class Event
{
public:
    Event() = default;
    virtual ~Event() = default;

public:
    virtual EventType::Enum GetEventType() const = 0;
    virtual const char* GetName() const = 0;
    virtual s32 GetEventCategories() const = 0;
    virtual std::string ToString() const { return GetName(); }
};

//////////////////////////////////////////////////////////////////////////

class SystemEventDispatcher
{
public:
    SystemEventDispatcher(Event& evt);

    template<typename Evt, typename Fun>
    bool Dispatch(Fun fun)
    {
        if (Evt::GetStaticType() == evt.GetEventType())
        {
            fun(static_cast<Evt&>(evt));
            return true;
        }

        return false;
    }

private:
    Event& evt;
};

//////////////////////////////////////////////////////////////////////////
