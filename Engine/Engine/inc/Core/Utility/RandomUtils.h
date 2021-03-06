//////////////////////////////////////////////////////////////////////////
//    File        	: RandomUtils.h
//    Created By    : Jack Spink
//    Created On 	: [1/11/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Singleton.h"

//////////////////////////////////////////////////////////////////////////

#include <random>

//////////////////////////////////////////////////////////////////////////

using IntegerDistribution = std::uniform_int_distribution<s32>;
using FloatDistribution = std::uniform_real_distribution<f32>;

//////////////////////////////////////////////////////////////////////////

struct global_random
{
public:
    static void initialise();

public:
    static f32 as_float(f32 min, f32 max);

private:
    static inline std::default_random_engine engine;
};

//////////////////////////////////////////////////////////////////////////

class Random
{
public:
    static void Seed(const u32 sd);

public:
    static f32 Float();
    static f32 Float(const f32 min, const f32 max);

private:
    static inline std::default_random_engine engine;
    static inline std::uniform_int_distribution<u32> distributor = std::uniform_int_distribution<u32>(0u, UINT_MAX);
};

//////////////////////////////////////////////////////////////////////////
