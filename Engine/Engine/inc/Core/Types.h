//////////////////////////////////////////////////////////////////////////
//    File        	: Types.h
//    Created By    : Jack Spink
//    Created On 	: [2/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

// 
// stl include
// 

#include <memory>
#include <cstdint>

// 
// glm include
// 

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

//////////////////////////////////////////////////////////////////////////

// 
// memory types
// 

template<typename Type>
using Ref = std::shared_ptr<Type>;

template<typename Type>
using WeakRef = std::weak_ptr<Type>;

template<typename Type, class...Types>
auto CreateRef = std::make_shared<Type, Types...>;

template<typename To, typename From>
auto RefCast = std::dynamic_pointer_cast<To, From>;

//////////////////////////////////////////////////////////////////////////

// 
// integer type defines
// 

//using byte = char;
//using boolean = bool;

using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;

using s8 = int8;
using s16 = int16;
using s32 = int32;
using s64 = int64;

using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

using u8 = uint8;
using u16 = uint16;
using u32 = uint32;
using u64 = uint64;

#ifdef _WIN64
using uint = uint64;
using sint = sint64;
#elif defined _WIN32
using uint = uint32;
using sint = int32;
#endif

// 
// floating point defines
// 

using float32 = float;
using float64 = double;

using f32 = float32;
using f64 = float64;

using real = f64;

// 
// glm defines
// 

using int1 = sint;
using int2 = glm::ivec2;
using int3 = glm::ivec3;
using int4 = glm::ivec4;

using uint1 = uint;
using uint2 = glm::uvec2;
using uint3 = glm::uvec3;
using uint4 = glm::uvec4;

using float1 = glm::fvec1;
using float2 = glm::fvec2;
using float3 = glm::fvec3;
using float4 = glm::fvec4;

using v2 = float2;
using v3 = float3;
using v4 = float4;

using double1 = f64;
using double2 = glm::dvec2;
using double3 = glm::dvec3;
using double4 = glm::dvec4;

using fmat3 = glm::mat3;
using fmat4 = glm::mat4;

using fquat = glm::fquat;

//////////////////////////////////////////////////////////////////////////

// math type defs

using vec2 = float2;
using vec3 = float3;
using vec4 = float4;

using mat3 = fmat3;
using mat4 = fmat4;

//////////////////////////////////////////////////////////////////////////

using GpuInt    = int32;
using GpuFloat  = f32;
using GpuFloat2 = float2;
using GpuFloat3 = float4;
using GpuFloat4 = float4;

//////////////////////////////////////////////////////////////////////////

using ClassId = s32;

//////////////////////////////////////////////////////////////////////////

static constexpr uint2 DefaultWindowSize = uint2(640u, 480u);
//static constexpr uint2 DefaultWindowSize = uint2(1280u, 720u);

//////////////////////////////////////////////////////////////////////////

#include <any>
#include <unordered_map>

//////////////////////////////////////////////////////////////////////////

class GenericBlueprint
{
public:
    using key = std::string;
    using value = std::any;
    using container = std::unordered_map<key, value>;
    using iterator = container::iterator;
    using const_iterator = container::const_iterator;

    inline iterator begin() { return bundle.begin(); }
    inline const_iterator begin() const { return bundle.begin(); }

    inline iterator end() { return bundle.end(); }
    inline const_iterator end() const { return bundle.end(); }

    inline iterator find(const key& k)
    {
        return bundle.find(k);
    }

    inline bool exists(const key& k)
    {
        return find(k) != bundle.end();
    }

    inline value& at(const key& k)
    {
        return bundle.at(k);
    }

    inline const value& at(const key& k) const
    {
        return bundle.at(k);
    }

    inline void add(const key& k, const value& v)
    {
        bundle.emplace(k, v);
    }

    inline bool remove(const key& k)
    {
        if (exists(k))
        {
            bundle.erase(k);
            return true;
        }

        return false;
    }

private:
    container bundle;
};

//////////////////////////////////////////////////////////////////////////
