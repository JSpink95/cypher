//////////////////////////////////////////////////////////////////////////
//    File        	: Core.h
//    Created By    : Jack Spink
//    Created On 	: [2/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include <memory>

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"
#include "Core/ConsoleLogger.h"

//////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
    #define ENABLE_ASSERTS
#endif

//////////////////////////////////////////////////////////////////////////

#ifdef ENABLE_ASSERTS
    #define APP_ASSERT(x, ...) { if(!(x)) { _ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
    #define APP_ASSERT(x, ...)
#endif

//////////////////////////////////////////////////////////////////////////

#define LOGPREP "[" __func__ "]"

//////////////////////////////////////////////////////////////////////////

#define BIND_FUNCTION_NoParam(Func)     std::bind(&Func, this)
#define BIND_FUNCTION_OneParam(Func)    std::bind(&Func, this, std::placeholders::_1)
#define BIND_FUNCTION_TwoParam(Func)    std::bind(&Func, this, std::placeholders::_1, std::placeholders::_2)
#define BIND_FUNCTION_ThreeParam(Func)  std::bind(&Func, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)

//////////////////////////////////////////////////////////////////////////

#ifdef _OPENGL

#define GlCall(code) code
#define Dx11Call(code)

#elif defined _DX11

#define GlCall(code)
#define Dx11Call(code) code

#else

#define GlCall(code)
#define Dx11Call(code)

#endif

//////////////////////////////////////////////////////////////////////////
