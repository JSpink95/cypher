//////////////////////////////////////////////////////////////////////////
//    File        	: CoreMaterialParameterBlocks.h
//    Created By    : Jack Spink
//    Created On 	: [9/10/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"
#include "Render/Platform/MaterialParameter.h"

//////////////////////////////////////////////////////////////////////////

#include <unordered_map>

//////////////////////////////////////////////////////////////////////////

struct MaterialFrameProperties
{
    fmat4 view, projection;
    fmat4 viewProjection;
};

//////////////////////////////////////////////////////////////////////////

struct MaterialObjectProperties
{
    fmat4 transform;
};

//////////////////////////////////////////////////////////////////////////

struct MaterialTimeProperties
{
    f32 time, deltaTime;
};

//////////////////////////////////////////////////////////////////////////

namespace EngineMaterialBlock
{
    enum Enum
    {
        PerFrame, PerObject, Time, Max
    };

    static inline const char* EnumToString[Max] =
    {
        "PerFrame", "PerObject", "Time",
    };

    static inline const size_t BlockByteSize[Max] =
    {
        sizeof(MaterialFrameProperties), sizeof(MaterialObjectProperties), sizeof(MaterialTimeProperties)
    };

    static inline std::unordered_map<std::string, Enum> NameToEnum =
    {
        std::make_pair(EnumToString[PerFrame], PerFrame),
        std::make_pair(EnumToString[PerObject], PerObject),
        std::make_pair(EnumToString[Time], Time),
    };
}

//////////////////////////////////////////////////////////////////////////
