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

#include "Render/Mesh.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Material.h"
#include "Render/Platform/VertexArray.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Utility/MaterialLibrary.h"
#include "Render/Utility/MeshLibrary.h"

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Component/StaticMeshComponent.h"

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

struct MetaTags
{
public:
    static inline const std::string Clamp = "Clamp";
    static inline const std::string Color = "Color";
};

//////////////////////////////////////////////////////////////////////////

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
        //std::stringstream ss(value);

        //while (ss >> result);

        std::string number;
        for (size_t idx = 0; idx < value.length(); ++idx)
        {
            const char digit = value.at(idx);
            if (digit == ',')
            {
                result.push_back(std::stof(number));
                number = "";
            }
            else
            {
                number += digit;
            }
        }

        if (number.length() != 0)
        {
            result.push_back(std::stof(number));
        }

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

class MetaParser
{
public:
    static void Parse(MetaData& data, const std::string& meta);
};

//////////////////////////////////////////////////////////////////////////

void MetaParser::Parse(MetaData& data, const std::string& meta)
{
    std::string name;
    for (u32 idx = 0; idx < meta.length(); ++idx)
    {
        const char current = meta.at(idx);
        if (current == '=')
        {
            idx += 1;
            std::string value;
            while (idx < meta.length() && meta.at(idx) != ' ')
            {
                value += meta.at(idx++);
            }

            data.meta.emplace(name, value);
            name = "";
        }
        else
        {
            name += meta.at(idx);
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void MetaData::LoadMetaData(const std::string& string)
{
    MetaParser::Parse(*this, string);
}

//////////////////////////////////////////////////////////////////////////

bool MetaData::HasMetaData(const std::string& id) const
{
    return meta.find(id) != meta.end();
}

//////////////////////////////////////////////////////////////////////////

float2 MetaData::GetAsFloat2(const std::string& id) const
{
    auto it = meta.find(id);
    if (it != meta.end())
    {
        std::string value = it->second.substr(1, it->second.length() - 2);
        return RTTI::ToFloat2(value);
    }

    return float2(-1.0f);
}

//////////////////////////////////////////////////////////////////////////

namespace ImGui
{
    bool VectorStringGetter(void* data, s32 index, const char** outText)
    {
        std::vector<std::string>& strings = *static_cast<std::vector<std::string>*>(data);
        if (index >= (s32)strings.size())
        {
            return false;
        }

        *outText = strings.at((size_t)index).c_str();
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
        if (prop->meta.HasMetaData(MetaTags::Clamp))
        {
            float2 minmax = prop->meta.GetAsFloat2(MetaTags::Clamp);
            return ImGui::DragFloat(id.c_str(), &editable, 1.0f, minmax.x, minmax.y);
        }
        else
        {
            return ImGui::InputFloat(id.c_str(), &editable);
        }
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool DisplayEditBox<float2>(void* owner, PropertyBase* prop, const std::string& id, float2& editable)
    {
        if (prop->meta.HasMetaData(MetaTags::Clamp))
        {
            float2 minmax = prop->meta.GetAsFloat2(MetaTags::Clamp);
            return ImGui::DragFloat2(id.c_str(), &editable.x, 1.0f, minmax.x, minmax.y);
        }
        else
        {
            return ImGui::InputFloat2(id.c_str(), &editable.x, 3);
        }
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool DisplayEditBox<float3>(void* owner, PropertyBase* prop, const std::string& id, float3& editable)
    {
        if (prop->meta.HasMetaData(MetaTags::Color))
        {
            return ImGui::ColorEdit3(id.c_str(), &editable.r);
        }
        else if (prop->meta.HasMetaData(MetaTags::Clamp))
        {
            float2 minmax = prop->meta.GetAsFloat2(MetaTags::Clamp);
            return ImGui::DragFloat3(id.c_str(), &editable.x, 1.0f, minmax.x, minmax.y);
        }
        else
        {
            return ImGui::InputFloat3(id.c_str(), &editable.x, 3);
        }
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool DisplayEditBox<float4>(void* owner, PropertyBase* prop, const std::string& id, float4& editable)
    {
        if (prop->meta.HasMetaData(MetaTags::Color))
        {
            return ImGui::ColorEdit4(id.c_str(), &editable.r);
        }
        else if (prop->meta.HasMetaData(MetaTags::Clamp))
        {
            float2 minmax = prop->meta.GetAsFloat2(MetaTags::Clamp);
            return ImGui::DragFloat4(id.c_str(), &editable.x, 1.0f, minmax.x, minmax.y);
        }
        else
        {
            return ImGui::InputFloat3(id.c_str(), &editable.x, 3);
        }
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

        std::vector<std::string> materials;
        MaterialLibrary::GetMaterialNames(materials);

        s32 index = std::distance(materials.begin(), std::find(materials.begin(), materials.end(), editable->GetPath()));
        const bool changed = ImGui::Combo(id.c_str(), &index, ImGui::VectorStringGetter, &materials, materials.size());

        if (changed)
        {
            editable = MaterialLibrary::GetMaterial(materials.at(index));
        }

        return changed;
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool DisplayEditBox<Ref<Mesh>>(void* owner, PropertyBase* prop, const std::string& id, Ref<Mesh>& editable)
    {
        // display a list of meshes...

        std::vector<std::string> meshes;
        MeshLibrary::GetMeshNames(meshes);

        const std::string name = editable ? editable->GetFilePath() : "";

        s32 index = std::distance(meshes.begin(), std::find(meshes.begin(), meshes.end(), name));
        const bool changed = ImGui::Combo(id.c_str(), &index, ImGui::VectorStringGetter, &meshes, meshes.size());

        if (changed)
        {
            editable = MeshLibrary::GetMesh(meshes.at(index));
        }

        return changed;
    }

    //////////////////////////////////////////////////////////////////////////

    template<> std::string ToString<bool>(const bool& value)
    {
        return value ? "true" : "false";
    }

    //////////////////////////////////////////////////////////////////////////

    template<> std::string ToString<s32>(const s32& value)
    {
        return std::to_string(value);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> std::string ToString<f32>(const f32& value)
    {
        return fmt::format("{0}", value);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> std::string ToString<float2>(const float2& value)
    {
        return fmt::format("{0},{1}", value.x, value.y);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> std::string ToString<float3>(const float3& value)
    {
        return fmt::format("{0},{1},{2}", value.x, value.y, value.z);
    }

    //////////////////////////////////////////////////////////////////////////

    template<> std::string ToString<std::string>(const std::string& value)
    {
        return value;
    }

    //////////////////////////////////////////////////////////////////////////

    template<> std::string ToString<Ref<Material>>(const Ref<Material>& value)
    {
        return value->GetPath();
    }

    //////////////////////////////////////////////////////////////////////////

    template<> std::string ToString<Ref<Mesh>>(const Ref<Mesh>& value)
    {
        return value->GetFilePath();
    }

    //////////////////////////////////////////////////////////////////////////

    template<> std::string ToString<ObjectId>(const ObjectId& value)
    {
        return value.GetStringId();
    }

    //////////////////////////////////////////////////////////////////////////

    template<> std::string ToString<ComponentId>(const ComponentId& value)
    {
        std::string result = value.GetStringId();
        result = result.substr(result.find_first_of(':') + 1);
        return result;
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool SetValue<bool>(const std::string& value, bool& editable)
    {
        editable = RTTI::ToBool(value);
        return true;
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool SetValue<s32>(const std::string& value, s32& editable)
    {
        editable = RTTI::ToInt32(value);
        return true;
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool SetValue<f32>(const std::string& value, f32& editable)
    {
        editable = RTTI::ToFloat(value);
        return true;
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool SetValue<float2>(const std::string& value, float2& editable)
    {
        editable = RTTI::ToFloat2(value);
        return true;
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool SetValue<float3>(const std::string& value, float3& editable)
    {
        editable = RTTI::ToFloat3(value);
        return true;
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool SetValue<std::string>(const std::string& value, std::string& editable)
    {
        editable = value;
        return true;
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool SetValue<Ref<Material>>(const std::string& value, Ref<Material>& editable)
    {
        editable = MaterialLibrary::GetMaterial(value);
        return true;
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool SetValue<Ref<Mesh>>(const std::string& value, Ref<Mesh>& editable)
    {
        editable = MeshLibrary::GetMesh(value);
        return true;
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool SetValue<ObjectId>(const std::string& value, ObjectId& editable)
    {
        editable = ObjectId::Create(value);
        return true;
    }

    //////////////////////////////////////////////////////////////////////////

    template<> bool SetValue<ComponentId>(const std::string& value, ComponentId& editable)
    {
        editable = ComponentId::Create(value);
        return true;
    }

    //////////////////////////////////////////////////////////////////////////
}

//////////////////////////////////////////////////////////////////////////

bool PropertyBase::IsRTTIObjectProperty() const
{
    return TypeRegister::IsRegisteredType(RTTI::TrimRefModifier(typeName));
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
    return RTTI::IsRefType(typeName);
}

//////////////////////////////////////////////////////////////////////////

void* PropertyBase::AsVoidPointer(void* base)
{
    char* bytes = (reinterpret_cast<char*>(base) + offset);
    return reinterpret_cast<void*>(bytes);
}

//////////////////////////////////////////////////////////////////////////

const void* PropertyBase::AsVoidPointer(void* base) const
{
    const char* bytes = (reinterpret_cast<const char*>(base) + offset);
    return reinterpret_cast<const void*>(bytes);
}

//////////////////////////////////////////////////////////////////////////

TypeBase* PropertyBase::GetType()
{
    return TypeRegister::GetRegisteredType(RTTI::TrimRefModifier(typeName));
}

//////////////////////////////////////////////////////////////////////////

RTTIObject* PropertyBase::AsRTTIObject(void* base)
{
    char* bytes = (reinterpret_cast<char*>(base) + offset);
    return reinterpret_cast<RTTIObject*>(bytes);
}

//////////////////////////////////////////////////////////////////////////

bool Property_ListBase::IsRTTIObjectProperty() const
{
    return false;
}

//////////////////////////////////////////////////////////////////////////

bool Property_ListBase::IsListProperty() const
{
    return true;
}

//////////////////////////////////////////////////////////////////////////

bool Property_ListBase::IsValueRefType() const
{
    return RTTI::IsRefType(valueTypeName);
}

//////////////////////////////////////////////////////////////////////////

bool Property_ListBase::IsValueRTTIObjectType() const
{
    return TypeRegister::IsRegisteredType(RTTI::TrimRefModifier(valueTypeName));
}

//////////////////////////////////////////////////////////////////////////

bool Property_MapBase::IsRTTIObjectProperty() const
{
    return false;
}

//////////////////////////////////////////////////////////////////////////

bool Property_MapBase::IsMapProperty() const
{
    return true;
}

//////////////////////////////////////////////////////////////////////////

bool Property_MapBase::IsKeyRefType() const
{
    return RTTI::IsRefType(keyTypeName);
}

//////////////////////////////////////////////////////////////////////////

bool Property_MapBase::IsKeyRTTIObjectType() const
{
    return TypeRegister::IsRegisteredType(RTTI::TrimRefModifier(keyTypeName));
}

//////////////////////////////////////////////////////////////////////////

bool Property_MapBase::IsValueRefType() const
{
    return RTTI::IsRefType(valueTypeName);
}

//////////////////////////////////////////////////////////////////////////

bool Property_MapBase::IsValueRTTIObjectType() const
{
    return TypeRegister::IsRegisteredType(RTTI::TrimRefModifier(valueTypeName));
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
