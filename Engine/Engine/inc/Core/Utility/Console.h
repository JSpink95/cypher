//////////////////////////////////////////////////////////////////////////
//    File        	: Console.h
//    Created By    : Jack Spink
//    Created On 	: [18/02/2020]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"

//////////////////////////////////////////////////////////////////////////

namespace Console
{
    enum Color : s32
    {
        Black       = 0x0,
        Blue        = 0x1,
        Green       = 0x2,
        Aqua        = 0x3,
        Red         = 0x4,
        Purple      = 0x5,
        Yellow      = 0x6,
        White       = 0x7,
        Gray        = 0x8,
        LightBlue   = 0x9,
        LightGreen  = 0xa,
        LightAqua   = 0xb,
        LightRed    = 0xc,
        LightPurple = 0xd,
        LightYellow = 0xe,
        BrightWhite = 0xf,
    };

    const auto Default = White;
    const auto Info = BrightWhite;
    const auto Warning = LightYellow;
    const auto Error = Red;

    void SetTextColor(Console::Color color);

    template<typename...Args>
    void Write(const char* fmt, Args... args)
    {
        printf(fmt, args...);
    }
}

//////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
    #define LOG_MESSAGE(...) Console::SetTextColor(Console::Default); Console::Write(__VA_ARGS__);
    #define LOG_INFO(...) Console::SetTextColor(Console::Info); Console::Write(__VA_ARGS__);
    #define LOG_WARNING(...) Console::SetTextColor(Console::Warning); Console::Write(__VA_ARGS__);
    #define LOG_ERROR(...) Console::SetTextColor(Console::Error); Console::Write(__VA_ARGS__);
#else
    #define LOG_MESSAGE(...)
    #define LOG_INFO(...)
    #define LOG_WARNING(...)
    #define LOG_ERROR(...)
#endif
//////////////////////////////////////////////////////////////////////////
