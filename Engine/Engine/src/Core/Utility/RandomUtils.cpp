//////////////////////////////////////////////////////////////////////////
//    File        	: RandomUtils.cpp
//    Created By    : Jack Spink
//    Created On 	: [1/11/2019]
//////////////////////////////////////////////////////////////////////////

#include "Core/Utility/RandomUtils.h"

//////////////////////////////////////////////////////////////////////////

#include <ctime>

//////////////////////////////////////////////////////////////////////////

void global_random::initialise()
{
    engine.seed(std::time(0));
}

//////////////////////////////////////////////////////////////////////////

f32 global_random::as_float(f32 min, f32 max)
{
    static IntegerDistribution dist(0, INT_MAX);

    f32 x = dist(engine) / (f32)INT_MAX;
    return (min + (max - min) * x);
}

//////////////////////////////////////////////////////////////////////////

void Random::Seed(const u32 sd)
{
    engine.seed(sd);
}

//////////////////////////////////////////////////////////////////////////

f32 Random::Float()
{
    return (f32)distributor(engine) / (f32)UINT_MAX;
}

//////////////////////////////////////////////////////////////////////////

f32 Random::Float(const f32 min, const f32 max)
{
    f32 random = Float();
    return min + random * (max - min);
}

//////////////////////////////////////////////////////////////////////////
