//////////////////////////////////////////////////////////////////////////
//    File        	: Container.h
//    Created By    : Jack Spink
//    Created On 	: [25/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"

//////////////////////////////////////////////////////////////////////////

class GuiContainer
{
public:
    GuiContainer(const f32 inX, const f32 inY, const f32 inWidth, const f32 inHeight);

    inline constexpr const f32 GetLeft() const { return x; }
    inline constexpr const f32 GetRight() const { return x + width; }
    inline constexpr const f32 GetTop() const { return y; }
    inline constexpr const f32 GetBottom() const { return y + height; }

    inline constexpr const float2 GetTopLeft() const { return float2(GetLeft(), GetTop()); }
    inline constexpr const float2 GetBottomRight() const { return float2(GetRight(), GetBottom()); }

    inline constexpr const f32 GetWidth() const { return width; }
    inline constexpr const f32 GetHeight() const { return height; }

private:
    f32 x, y;
    f32 width, height;
};

//////////////////////////////////////////////////////////////////////////
