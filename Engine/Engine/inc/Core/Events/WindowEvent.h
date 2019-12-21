//////////////////////////////////////////////////////////////////////////
//    File        	: WindowEvent.h
//    Created By    : Jack Spink
//    Created On 	: [3/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Events/Event.h"

//////////////////////////////////////////////////////////////////////////

class WindowResizeEvent : public Event
{
public:
    WindowResizeEvent(const uint2& resolution);

    inline const uint2& GetResolution() const { return resolution; }

public:
    EVENT_CATEGORY_TYPE(EventCategory::Window);
    EVENT_CLASS_TYPE(WindowResize)

private:
    uint2 resolution;
};

//////////////////////////////////////////////////////////////////////////

class WindowCloseEvent : public Event
{
public:
    WindowCloseEvent() = default;

public:
    EVENT_CATEGORY_TYPE(EventCategory::Window);
    EVENT_CLASS_TYPE(WindowClose)

};

//////////////////////////////////////////////////////////////////////////
