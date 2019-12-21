//////////////////////////////////////////////////////////////////////////
//    File        	: Constraint.h
//    Created By    : Jack Spink
//    Created On 	: [25/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"

//////////////////////////////////////////////////////////////////////////

class GuiContainer;

//////////////////////////////////////////////////////////////////////////

class GuiConstraint
{
public:
    virtual f32 Solve(const f32 base) const = 0;
};

//////////////////////////////////////////////////////////////////////////

class GuiLayout
{
public:
    GuiLayout(GuiContainer* container);

    inline void SetXConstraint(Ref<GuiConstraint> constraint) { xConstraint = constraint; }
    inline void SetYConstraint(Ref<GuiConstraint> constraint) { yConstraint = constraint; }
    inline void SetWidthConstraint(Ref<GuiConstraint> constraint) { widthConstraint = constraint; }
    inline void SetHeightConstraint(Ref<GuiConstraint> constraint) { heightConstraint = constraint; }

    const f32 GetX() const;
    const f32 GetY() const;
    const f32 GetWidth() const;
    const f32 GetHeight() const;

private:
    GuiContainer* owner;
    Ref<GuiConstraint> xConstraint, yConstraint;
    Ref<GuiConstraint> widthConstraint, heightConstraint;
};

//////////////////////////////////////////////////////////////////////////
