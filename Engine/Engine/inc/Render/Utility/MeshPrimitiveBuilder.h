//////////////////////////////////////////////////////////////////////////
//    File        	: MeshPrimitiveBuilder.h
//    Created By    : Jack Spink
//    Created On 	: [24/10/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"

//////////////////////////////////////////////////////////////////////////

#include <vector>

//////////////////////////////////////////////////////////////////////////

namespace MeshPrimitiveBuilder
{
    using FaceFlag = s32;

    constexpr FaceFlag const TopFace      = 0b000001;
    constexpr FaceFlag const BottomFace   = 0b000010;
    constexpr FaceFlag const LeftFace     = 0b000100;
    constexpr FaceFlag const RightFace    = 0b001000;
    constexpr FaceFlag const FrontFace    = 0b010000;
    constexpr FaceFlag const BackFace     = 0b100000;

    constexpr FaceFlag const NoFaces = 0b0;
    constexpr FaceFlag const AllFaces = TopFace | BottomFace | LeftFace | RightFace | FrontFace | BackFace;

    struct PosNorTex
    {
        vec3 p, n;
        vec2 t;
    };

    void CreateBox(const float3& centre, const float3& halfSize, std::vector<PosNorTex>& pnt, FaceFlag const flags = AllFaces);
}

//////////////////////////////////////////////////////////////////////////
