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

#include "glm/glm.hpp"
#include "imgui.h"

//////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>

//////////////////////////////////////////////////////////////////////////

// #todo - check if this actually works, if not make a brute force float parse
inline std::stringstream& operator>>(std::stringstream& ss, std::vector<f32>& output)
{
    f32 value = 0.0f;
    ss >> value;
    output.push_back(value);

    return ss;
}

//////////////////////////////////////////////////////////////////////////

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

    inline float4 ToFloat4(const std::string& value)
    {
        std::vector<f32> floats = GetFloatList(value);
        // assert
        return float4(floats.at(0), floats.at(1), floats.at(2), floats.at(3));
    }
}

//////////////////////////////////////////////////////////////////////////

namespace RTTI
{
    template<> void SetValueFromString<bool>(const std::string& string, bool& editable)
    {
        editable = RTTI::ToBool(string);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> void ShowEditBox<bool>(const std::string& id, bool& editable)
    {
        ImGui::Checkbox(id.c_str(), &editable);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> void SetValueFromString<s32>(const std::string& string, s32& editable)
    {
        editable = RTTI::ToInt32(string);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> void ShowEditBox<s32>(const std::string& id, s32& editable)
    {
        ImGui::InputInt(id.c_str(), &editable);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> void SetValueFromString<f32>(const std::string& string, f32& editable)
    {
        editable = RTTI::ToFloat(string);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> void ShowEditBox<f32>(const std::string& id, f32& editable)
    {
        ImGui::InputFloat(id.c_str(), &editable);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> void SetValueFromString<float2>(const std::string& string, float2& editable)
    {
        editable = RTTI::ToFloat2(string);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> void ShowEditBox<float2>(const std::string& id, float2& editable)
    {
        ImGui::InputFloat2(id.c_str(), &editable.x, 3);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> void SetValueFromString<float3>(const std::string& string, float3& editable)
    {
        editable = RTTI::ToFloat3(string);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> void ShowEditBox<float3>(const std::string& id, float3& editable)
    {
        ImGui::InputFloat3(id.c_str(), &editable.x, 3);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> void SetValueFromString<std::string>(const std::string& string, std::string& editable)
    {
        editable = string;
    }

    //////////////////////////////////////////////////////////////////////////

    template<> void ShowEditBox<std::string>(const std::string& id, std::string& editable)
    {
        static constexpr const size_t maxBufferSize = 128u;

        char buffer[maxBufferSize];
        std::memcpy(buffer, editable.c_str(), glm::min(maxBufferSize, editable.length()));

        ImGui::InputText(id.c_str(), buffer, maxBufferSize);

        editable = buffer;
    }

    //////////////////////////////////////////////////////////////////////////

    template<> void SetValueFromString<Ref<Material>>(const std::string& string, Ref<Material>& editable)
    {
        editable = MaterialLibrary::GetMaterial(string);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> void ShowEditBox<Ref<Material>>(const std::string& id, Ref<Material>& editable)
    {
        // display a list of materials...
        ImGui::Text("Not yet implemented - MaterialRef");
    }

    //////////////////////////////////////////////////////////////////////////

    template<> void SetValueFromString<Ref<VertexArray>>(const std::string& string, Ref<VertexArray>& editable)
    {
        editable = MeshLibrary::GetMesh(string);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> void ShowEditBox<Ref<VertexArray>>(const std::string& id, Ref<VertexArray>& editable)
    {
        // display a list of meshes...
        ImGui::Text("Not yet implemented - VertexArrayRef");
    }
}

//////////////////////////////////////////////////////////////////////////
