//////////////////////////////////////////////////////////////////////////
//    File        	: ConsoleLogger.h
//    Created By    : Jack Spink
//    Created On 	: [3/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#define SPDLOG_COMPILED_LIB
#include "spdlog/spdlog.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"

//////////////////////////////////////////////////////////////////////////

class ConsoleLogger
{
public:
    static void Init();

    inline static Ref<spdlog::logger>& GetClientLogger() { return logger; }

private:
    static Ref<spdlog::logger> logger;
};

//////////////////////////////////////////////////////////////////////////

#define _TRACE(...)         ConsoleLogger::GetClientLogger()->trace(__VA_ARGS__)
#define _INFO(...)          ConsoleLogger::GetClientLogger()->info(__VA_ARGS__)
#define _WARN(...)          ConsoleLogger::GetClientLogger()->warn(__VA_ARGS__)
#define _ERROR(...)         ConsoleLogger::GetClientLogger()->error(__VA_ARGS__)
#define _CRITICAL(...)      ConsoleLogger::GetClientLogger()->critical(__VA_ARGS__)

//////////////////////////////////////////////////////////////////////////
