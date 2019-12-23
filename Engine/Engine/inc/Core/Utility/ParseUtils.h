//////////////////////////////////////////////////////////////////////////
//    File        	: ParseUtils.h
//    Created By    : Jack Spink
//    Created On 	: [23/12/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"

//////////////////////////////////////////////////////////////////////////

#include <string>

//////////////////////////////////////////////////////////////////////////

template<typename TFloat>
inline TFloat ParseFloatString(const std::string& floats)
{
    u32 component = 0;
    TFloat result = TFloat(0.0f);

    std::string number = "";
    for (u32 idx = 0; idx < floats.length(); ++idx)
    {
        const char c = floats.at(idx);
        if (isdigit(c) || c == '.' || c == '-')
            number += c;

        if (c == ',' || idx == (floats.length() - 1))
        {
            result[component++] = std::stof(number);
            number = "";
        }
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////

constexpr auto ParseFloat2 = ParseFloatString<float2>;
constexpr auto ParseFloat3 = ParseFloatString<float3>;
constexpr auto ParseFloat4 = ParseFloatString<float4>;

//////////////////////////////////////////////////////////////////////////
