//////////////////////////////////////////////////////////////////////////
//    File        	: Console.cpp
//    Created By    : Jack Spink
//    Created On 	: [18/02/2020]
//////////////////////////////////////////////////////////////////////////

#include "Core/Utility/Console.h"

//////////////////////////////////////////////////////////////////////////

#include <windows.h>

//////////////////////////////////////////////////////////////////////////

namespace Console
{
    void SetTextColor(Console::Color color)
    {
        HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(handle, (s32)color);
    }
}

//////////////////////////////////////////////////////////////////////////
