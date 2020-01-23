//////////////////////////////////////////////////////////////////////////
//    File        	: PropertRegister.cpp
//    Created By    : Jack Spink
//    Created On 	: [13/1/2020]
//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/PropertyRegister.h"
#include "Core/RTTI/RTTIObject.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Object.h"
#include "Core/ComponentRef.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Material.h"
#include "Render/Platform/VertexArray.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Utility/MaterialLibrary.h"
#include "Render/Utility/MeshLibrary.h"

//////////////////////////////////////////////////////////////////////////

#include "Particle/ParticleUpdater.h"

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

namespace ImGui
{
    bool VectorStringGetter(void* data, s32 index, const char** outText)
    {
        std::vector<std::string>& strings = *static_cast<std::vector<std::string>*>(data);
        if (index >= strings.size())
        {
            return false;
        }

        *outText = strings.at(index).c_str();
        return true;
    }

    bool VectorStringList(const std::string& label, std::vector<std::string>& strings, s32& selected)
    {
        return ImGui::ListBox(label.c_str(), &selected, VectorStringGetter, &strings, strings.size());
    }

    bool VectorStringComboBox(const std::string& label, std::vector<std::string>& strings, s32& selected)
    {
        return ImGui::Combo(label.c_str(), &selected, VectorStringGetter, &strings, strings.size());
    }
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
    //////////////////////////////////////////////////////////////////////////

    bool IsRefType(const std::string& typeName)
    {
        return (typeName.rfind("Ref<", 0) == 0);
    }

    //////////////////////////////////////////////////////////////////////////

    std::string TrimRefModifier(const std::string& typeName)
    {
        std::string result = typeName;

        if (IsRefType(result))
        {
            // remove Ref< from start
            result = result.substr(4);

            // Remove > from end
            result.pop_back();
        }

        return result;
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool DisplayEditBox<bool>(void* owner, PropertyBase* prop, const std::string& id, bool& editable)
    {
        return ImGui::Checkbox(id.c_str(), &editable);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool DisplayEditBox<s32>(void* owner, PropertyBase* prop, const std::string& id, s32& editable)
    {
        return ImGui::InputInt(id.c_str(), &editable);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool DisplayEditBox<f32>(void* owner, PropertyBase* prop, const std::string& id, f32& editable)
    {
        return ImGui::InputFloat(id.c_str(), &editable);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool DisplayEditBox<float2>(void* owner, PropertyBase* prop, const std::string& id, float2& editable)
    {
        return ImGui::InputFloat2(id.c_str(), &editable.x, 3);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool DisplayEditBox<float3>(void* owner, PropertyBase* prop, const std::string& id, float3& editable)
    {
        return ImGui::InputFloat3(id.c_str(), &editable.x, 3);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool DisplayEditBox<std::string>(void* owner, PropertyBase* prop, const std::string& id, std::string& editable)
    {
        static constexpr const size_t maxBufferSize = 128u;

        char buffer[maxBufferSize];
        std::memcpy(buffer, editable.c_str(), glm::min(maxBufferSize, editable.length()));

        const bool changed = ImGui::InputText(id.c_str(), buffer, maxBufferSize);

        editable = buffer;

        return changed;
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool DisplayEditBox<Ref<Material>>(void* owner, PropertyBase* prop, const std::string& id, Ref<Material>& editable)
    {
        // display a list of materials...
        ImGui::Text("Not yet implemented - MaterialRef");
        return false;
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool DisplayEditBox<Ref<VertexArray>>(void* owner, PropertyBase* prop, const std::string& id, Ref<VertexArray>& editable)
    {
        // display a list of meshes...
        ImGui::Text("Not yet implemented - VertexArrayRef");
        return false;
    }

    //////////////////////////////////////////////////////////////////////////
}

//////////////////////////////////////////////////////////////////////////

bool PropertyBase::IsRTTIObjectProperty() const
{
    return TypeRegister::IsRegisteredTypeOf(RTTI::TrimRefModifier(propertyName), RTTIObject::ClassUID());
}

//////////////////////////////////////////////////////////////////////////

bool PropertyBase::IsListProperty() const
{
    return false;
}

//////////////////////////////////////////////////////////////////////////

bool PropertyBase::IsMapProperty() const
{
    return false;
}

//////////////////////////////////////////////////////////////////////////

bool PropertyBase::IsRefType() const
{
    // this needs some thought...
    return RTTI::IsRefType(propertyName);
}

//////////////////////////////////////////////////////////////////////////

void* PropertyBase::AsVoidPointer(void* base)
{
    char* bytes = (reinterpret_cast<char*>(base) + offset);
    return (void*)bytes;
}

//////////////////////////////////////////////////////////////////////////

const void* PropertyBase::AsVoidPointer(void* base) const
{
    const char* bytes = (reinterpret_cast<const char*>(base) + offset);
    return (const void*)bytes;
}

//////////////////////////////////////////////////////////////////////////

RTTIObject* PropertyBase::AsRTTIObject(void* base)
{
    char* bytes = (reinterpret_cast<char*>(base) + offset);
    return (RTTIObject*)bytes;
}

//////////////////////////////////////////////////////////////////////////

TypeBase* Property_MapBase::GetKeyType()
{
    return TypeRegister::GetRegisteredType(keyTypeName);
}

//////////////////////////////////////////////////////////////////////////

TypeBase* Property_MapBase::GetValueType()
{
    return TypeRegister::GetRegisteredType(RTTI::TrimRefModifier(valueTypeName));
}

//////////////////////////////////////////////////////////////////////////
