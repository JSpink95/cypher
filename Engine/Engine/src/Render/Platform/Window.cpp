//////////////////////////////////////////////////////////////////////////
//    File        	: Window.cpp
//    Created By    : Jack Spink
//    Created On 	: [3/9/2019]
//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Window.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Application.h"

//////////////////////////////////////////////////////////////////////////

uint2 Display::GetSize()
{
    Application* app = GetApplication();
    Window* window = app ? app->GetWindowContext().get() : nullptr;

    if (window != nullptr)
    {
        return window->GetWindowSize();
    }

    return uint2(1);
}

//////////////////////////////////////////////////////////////////////////

void Display::GetSupportedDimensions(std::vector<uint2>& dimensions)
{
    Application* app = GetApplication();
    Window* window = app ? app->GetWindowContext().get() : nullptr;

    if (window != nullptr)
    {
        window->GetSupportedDimensions(dimensions);
    }
}

//////////////////////////////////////////////////////////////////////////
