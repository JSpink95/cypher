//////////////////////////////////////////////////////////////////////////
//    File        	: PropertRegister.cpp
//    Created By    : Jack Spink
//    Created On 	: [13/1/2020]
//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/PropertyRegister.h"

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
    template<> void SetValueFromString<bool>(const std::string& string, bool& editable)
    {
        editable = RTTI::ToBool(string);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool ShowEditBox<bool>(void* owner, BaseProperty* prop, const std::string& id, bool& editable)
    {
        return ImGui::Checkbox(id.c_str(), &editable);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> void SetValueFromString<s32>(const std::string& string, s32& editable)
    {
        editable = RTTI::ToInt32(string);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool ShowEditBox<s32>(void* owner, BaseProperty* prop, const std::string& id, s32& editable)
    {
        return ImGui::InputInt(id.c_str(), &editable);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> void SetValueFromString<f32>(const std::string& string, f32& editable)
    {
        editable = RTTI::ToFloat(string);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool ShowEditBox<f32>(void* owner, BaseProperty* prop, const std::string& id, f32& editable)
    {
        return ImGui::InputFloat(id.c_str(), &editable);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> void SetValueFromString<float2>(const std::string& string, float2& editable)
    {
        editable = RTTI::ToFloat2(string);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool ShowEditBox<float2>(void* owner, BaseProperty* prop, const std::string& id, float2& editable)
    {
        return ImGui::InputFloat2(id.c_str(), &editable.x, 3);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> void SetValueFromString<float3>(const std::string& string, float3& editable)
    {
        editable = RTTI::ToFloat3(string);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool ShowEditBox<float3>(void* owner, BaseProperty* prop, const std::string& id, float3& editable)
    {
        return ImGui::InputFloat3(id.c_str(), &editable.x, 3);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> void SetValueFromString<std::string>(const std::string& string, std::string& editable)
    {
        editable = string;
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool ShowEditBox<std::string>(void* owner, BaseProperty* prop, const std::string& id, std::string& editable)
    {
        static constexpr const size_t maxBufferSize = 128u;

        char buffer[maxBufferSize];
        std::memcpy(buffer, editable.c_str(), glm::min(maxBufferSize, editable.length()));

        const bool changed = ImGui::InputText(id.c_str(), buffer, maxBufferSize);

        editable = buffer;

        return changed;
    }

    //////////////////////////////////////////////////////////////////////////

    template<> void SetValueFromString<Ref<Material>>(const std::string& string, Ref<Material>& editable)
    {
        editable = MaterialLibrary::GetMaterial(string);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool ShowEditBox<Ref<Material>>(void* owner, BaseProperty* prop, const std::string& id, Ref<Material>& editable)
    {
        // display a list of materials...
        ImGui::Text("Not yet implemented - MaterialRef");
        return false;
    }

    //////////////////////////////////////////////////////////////////////////

    template<> void SetValueFromString<Ref<VertexArray>>(const std::string& string, Ref<VertexArray>& editable)
    {
        editable = MeshLibrary::GetMesh(string);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool ShowEditBox<Ref<VertexArray>>(void* owner, BaseProperty* prop, const std::string& id, Ref<VertexArray>& editable)
    {
        // display a list of meshes...
        ImGui::Text("Not yet implemented - VertexArrayRef");
        return false;
    }

    //////////////////////////////////////////////////////////////////////////

    bool ShowComponentRefEditBox(void* owner, BaseProperty* prop, ClassId classId, const std::string& id, ComponentRefBase* editable)
    {
        Component* ownerAsComponent = (Component*)owner;
        Object* componentOwner = ownerAsComponent != nullptr ? ownerAsComponent->GetOwner() : nullptr;
        if (componentOwner)
        {
            std::vector<std::string> componentNames;
            componentOwner->ForEachComponent([&componentNames, classId](ComponentId id, Ref<Component> component) -> void
            {
                if (component->IsTypeOf(classId))
                {
                    const std::string& componentName = id.GetStringId();
                    const std::string trimmedComponentName = componentName.substr(componentName.find_first_of(":") + 1);

                    componentNames.push_back(trimmedComponentName);
                }
            });

            auto it = std::find(componentNames.begin(), componentNames.end(), editable->componentName);

            s32 selected = it != componentNames.end() ? std::distance(componentNames.begin(), it) : 0;
            
            const bool changed = ImGui::VectorStringComboBox(id, componentNames, selected);

            if (changed)
            {
                editable->componentName = componentNames.at(selected);
                editable->OnComponentChanged(componentOwner);
            }

            return changed;
        }

        return false;
    }
}

//////////////////////////////////////////////////////////////////////////
