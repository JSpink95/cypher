//////////////////////////////////////////////////////////////////////////
//    File        	: PropertRegister.cpp
//    Created By    : Jack Spink
//    Created On 	: [13/1/2020]
//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/PropertyRegister.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Material.h"
#include "Render/Platform/VertexArray.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Utility/MaterialLibrary.h"
#include "Render/Utility/MeshLibrary.h"

//////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>

//////////////////////////////////////////////////////////////////////////

inline std::stringstream& operator>>(std::stringstream& ss, std::vector<f32>& output)
{
    f32 value = 0.0f;
    ss >> value;
    output.push_back(value);

    return ss;
}

namespace RTTI
{
    std::vector<f32> GetFloatList(const std::string& value)
    {
        std::vector<f32> result;
        std::stringstream ss(value);

        while (ss >> result);

        return result;
    }

    inline bool ToBool(const std::string& value)
    {
        std::string converted = value;
        std::transform(converted.begin(), converted.end(), converted.begin(), tolower);

        // everything else is false
        return converted == "true";
    }

    inline s32 ToInt32(const std::string& value)
    {
        return std::stoi(value);
    }

    inline f32 ToFloat(const std::string& value)
    {
        return std::stof(value);
    }

    inline float2 ToFloat2(const std::string& value)
    {
        std::vector<f32> floats = GetFloatList(value);
        // assert
        return float2(floats.at(0), floats.at(1));
    }

    inline float3 ToFloat3(const std::string& value)
    {
        std::vector<f32> floats = GetFloatList(value);
        // assert
        return float3(floats.at(0), floats.at(1), floats.at(2));
    }
}

//////////////////////////////////////////////////////////////////////////

template<> void Property<bool>::SetFromString(void* base, const std::string& string)
{
    bool value = RTTI::ToBool(string);
    SetFromStrongType(base, value);
}

//////////////////////////////////////////////////////////////////////////

template<> void Property<s32>::SetFromString(void* base, const std::string& string)
{
    s32 value = RTTI::ToInt32(string);
    SetFromStrongType(base, value);
}

//////////////////////////////////////////////////////////////////////////

template<> void Property<f32>::SetFromString(void* base, const std::string& string)
{
    f32 value = RTTI::ToFloat(string);
    SetFromStrongType(base, value);
}

//////////////////////////////////////////////////////////////////////////

template<> void Property<float2>::SetFromString(void* base, const std::string& string)
{
    float2 value = RTTI::ToFloat2(string);
    SetFromStrongType(base, value);
}

//////////////////////////////////////////////////////////////////////////

template<> void Property<float3>::SetFromString(void* base, const std::string& string)
{
    float3 value = RTTI::ToFloat3(string);
    SetFromStrongType(base, value);
}

//////////////////////////////////////////////////////////////////////////

template<> void Property<std::string>::SetFromString(void* base, const std::string& string)
{
    SetFromStrongType(base, string);
}

//////////////////////////////////////////////////////////////////////////

template<> void RefProperty<Material>::SetFromString(void* base, const std::string& string)
{
    Ref<Material> material = MaterialLibrary::GetMaterial(string);
    SetFromStrongType(base, material);
}

//////////////////////////////////////////////////////////////////////////

template<> void RefProperty<VertexArray>::SetFromString(void* base, const std::string& string)
{
    Ref<VertexArray> mesh = MeshLibrary::GetMesh(string);
    SetFromStrongType(base, mesh);
}

//////////////////////////////////////////////////////////////////////////
