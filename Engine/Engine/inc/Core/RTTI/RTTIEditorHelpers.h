//////////////////////////////////////////////////////////////////////////
//    File        	: RTTIEditorHelpers.h
//    Created By    : Jack Spink
//    Created On 	: [07/02/2020]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Utility/Console.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/RTTIObject.h"
#include "Core/RTTI/TypeRegister.h"

//////////////////////////////////////////////////////////////////////////

#include "imgui.h"

//////////////////////////////////////////////////////////////////////////

#include <string>
#include <algorithm>

//////////////////////////////////////////////////////////////////////////

class Mesh;
class Material;

//////////////////////////////////////////////////////////////////////////

namespace RTTI
{
    namespace Helper
    {
        bool ImGuiStringGetter(void* data, s32 idx, const char** out);
    }

    //////////////////////////////////////////////////////////////////////////

    template<typename TValue>
    void DisplayEdit(const std::string& id, TValue& value);

    //////////////////////////////////////////////////////////////////////////

    template<template<typename> typename TList, typename TValue>
    void DisplayListEdit(const std::string& id, TList<TValue>& list, s32& selected);

    template<template<typename, typename> typename TMap, typename TKey, typename TValue>
    void DisplayMapEdit(const std::string& id, TMap<TKey, TValue>& map);

//////////////////////////////////////////////////////////////////////////
// specialised displays

template<> void DisplayEdit<bool>(const std::string& id, bool& editable);
template<> void DisplayEdit<s32>(const std::string& id, s32& editable);
template<> void DisplayEdit<f32>(const std::string& id, f32& editable);
template<> void DisplayEdit<float2>(const std::string& id, float2& editable);
template<> void DisplayEdit<float3>(const std::string& id, float3& editable);
template<> void DisplayEdit<std::string>(const std::string& id, std::string& editable);
template<> void DisplayEdit<Ref<Material>>(const std::string& id, Ref<Material>& editable);
template<> void DisplayEdit<Ref<Mesh>>(const std::string& id, Ref<Mesh>& editable);
template<> void DisplayEdit<Ref<RTTIObject>>(const std::string& id, Ref<RTTIObject>& value);

//////////////////////////////////////////////////////////////////////////

}

//////////////////////////////////////////////////////////////////////////

template<typename TValue>
void RTTI::DisplayEdit(const std::string& id, TValue& value)
{
    LOG_WARNING("Not implemented for this type.\n");
}

//////////////////////////////////////////////////////////////////////////

template<template<typename> typename TList, typename TValue>
void RTTI::DisplayListEdit(const std::string& id, TList<TValue>& list, s32& selected)
{
    if (ImGui::TreeNode(id.c_str()))
    {
        std::vector<TypeBase*> types;
        TypeRegister::GetRegisteredTypesOfBase<TValue::element_type>(types);

        std::vector<std::string> strings;
        std::transform(types.begin(), types.end(), std::back_inserter(strings), [](TypeBase* type) -> std::string { return type->GetTypeName(); });

        ImGui::Combo("##list_types", &selected, RTTI::Helper::ImGuiStringGetter, &strings, strings.size());
        ImGui::SameLine();

        const bool add = ImGui::Button("+##add_item");

        std::vector<TList<TValue>::iterator> idsToRemove;
        for (s32 index = 0; index < list.size(); ++index)
        {
            const std::string elementId = std::to_string(index);
            const std::string removeId = "-##id_" + elementId;

            Ref<RTTIObject> value = list.at(index);

            //if (ImGui::TreeNode(elementId.c_str()))
            //{
            //    ImGui::TreePop();
            //}

            const bool remove = ImGui::Button(removeId.c_str());
            ImGui::SameLine();

            RTTI::DisplayEdit<Ref<RTTIObject>>("", value);

            if (remove)
            {
                idsToRemove.push_back(list.begin() + index);
            }
        }

        if (add)
        {
            TypeBase* newType = types.at(selected);

            Ref<TValue::element_type> element;
            element.reset((TValue::element_type*)newType->New());

            list.push_back(element);
        }

        if (idsToRemove.size() > 0)
        {
            //std::sort(idsToRemove.begin(), idsToRemove.end(), std::less<s32>());
            for (s32 idx = idsToRemove.size() - 1; idx >= 0; --idx)
            {
                TList<TValue>::iterator it = idsToRemove.at((u32)idx);
                list.erase(it);
            }
        }

        ImGui::TreePop();
    }
}

//////////////////////////////////////////////////////////////////////////

template<template<typename, typename> typename TMap, typename TKey, typename TValue>
void RTTI::DisplayMapEdit(const std::string& id, TMap<TKey, TValue>& map)
{
    s32 element = 0;
    for (auto&[key, value] : map)
    {
        const std::string keyId = std::to_string(element) + ":key";
        const std::string valueId = std::to_string(element) + ":value";

        if (ImGui::TreeNode(keyId.c_str()))
        {
            RTTI::DisplayEdit<Ref<RTTIObject>>(valueId, std::dynamic_pointer_cast<RTTIObject>(value));
            ImGui::TreePop();
        }

        element += 1;
    }
}

//////////////////////////////////////////////////////////////////////////