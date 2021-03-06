//////////////////////////////////////////////////////////////////////////
//    File        	: FixedConstraint.cpp
//    Created By    : Jack Spink
//    Created On 	: [25/9/2019]
//////////////////////////////////////////////////////////////////////////

#include "Gui/Constraint/FixedConstraint.h"

//////////////////////////////////////////////////////////////////////////

FixedGuiConstraint::FixedGuiConstraint(const f32 inValue)
    : value(inValue)
{

}

//////////////////////////////////////////////////////////////////////////

f32 FixedGuiConstraint::Solve(const f32 base) const
{
    return base + value;
}

//////////////////////////////////////////////////////////////////////////
