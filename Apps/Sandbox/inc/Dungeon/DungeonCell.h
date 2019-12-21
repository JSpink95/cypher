//////////////////////////////////////////////////////////////////////////
//    File        	: DungeonCell.h
//    Created By    : Jack Spink
//    Created On 	: [7/10/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"

//////////////////////////////////////////////////////////////////////////

class DungeonCell
{
public:
    static inline constexpr const f32 Height = 1.75f;
    static inline constexpr const f32 HalfHeight = Height / 2.0f;

    enum Type
    {
        Floor, Wall, Hole, Max
    };

public:
    DungeonCell(const s32 ix, const s32 iz);

    inline s32 GetX() const { return x; }
    inline s32 GetZ() const { return z; }
    inline Type GetType() const { return type; }

    inline void SetType(const Type newType) { type = newType; }
private:
    s32 x, z;
    Type type = Floor;
};

//////////////////////////////////////////////////////////////////////////
