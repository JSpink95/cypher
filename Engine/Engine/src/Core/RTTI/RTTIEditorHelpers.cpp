//////////////////////////////////////////////////////////////////////////
//    File        	: RTTIEditorHelpers.cpp
//    Created By    : Jack Spink
//    Created On 	: [07/02/2020]
//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/RTTIEditorHelpers.h"
#include "Core/RTTI/PropertyRegister.h"
#include "Core/RTTI/RTTIObject.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Mesh.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Material.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Utility/MeshLibrary.h"
#include "Render/Utility/MaterialLibrary.h"

//////////////////////////////////////////////////////////////////////////

namespace RTTI
{
    //////////////////////////////////////////////////////////////////////////

    namespace Helper
    {
        bool ImGuiStringGetter(void* data, s32 idx, const char** out)
        {
            std::vector<std::string>* strings = reinterpret_cast<std::vector<std::string>*>(data);
            if (idx >= 0 && idx < strings->size())
            {
                *out = strings->at(idx).c_str();
                return true;
            }

            return false;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    
    template<> void DisplayEdit<bool>(const std::string& id, bool& editable)
    {
        ImGui::Checkbox(id.c_str(), &editable);
    }
    
    //////////////////////////////////////////////////////////////////////////
    
    template<> void DisplayEdit<s32>(const std::string& id, s32& editable)
    {
        ImGui::InputInt(id.c_str(), &editable);
    }
    
    //////////////////////////////////////////////////////////////////////////
    
    template<> void DisplayEdit<f32>(const std::string& id, f32& editable)
    {
        //if (prop->meta.HasMetaData(MetaTags::Clamp))
        //{
        //    float2 minmax = prop->meta.GetAsFloat2(MetaTags::Clamp);
        //    ImGui::DragFloat(id.c_str(), &editable, 1.0f, minmax.x, minmax.y);
        //}
        //else
        {
            ImGui::InputFloat(id.c_str(), &editable);
        }
    }
    
    //////////////////////////////////////////////////////////////////////////
    
    template<> void DisplayEdit<float2>(const std::string& id, float2& editable)
    {
        //if (prop->meta.HasMetaData(MetaTags::Clamp))
        //{
        //    float2 minmax = prop->meta.GetAsFloat2(MetaTags::Clamp);
        //    ImGui::DragFloat2(id.c_str(), &editable.x, 1.0f, minmax.x, minmax.y);
        //}
        //else
        {
            ImGui::InputFloat2(id.c_str(), &editable.x, 3);
        }
    }
    
    //////////////////////////////////////////////////////////////////////////
    
    template<> void DisplayEdit<float3>(const std::string& id, float3& editable)
    {
        //if (prop->meta.HasMetaData(MetaTags::Color))
        //{
        //    ImGui::ColorEdit3(id.c_str(), &editable.r);
        //}
        //else if (prop->meta.HasMetaData(MetaTags::Clamp))
        //{
        //    float2 minmax = prop->meta.GetAsFloat2(MetaTags::Clamp);
        //    ImGui::DragFloat3(id.c_str(), &editable.x, 1.0f, minmax.x, minmax.y);
        //}
        //else
        {
            ImGui::InputFloat3(id.c_str(), &editable.x, 3);
        }
    }
    
    //////////////////////////////////////////////////////////////////////////
    
    template<> void DisplayEdit<float4>(const std::string& id, float4& editable)
    {
        //if (prop->meta.HasMetaData(MetaTags::Color))
        //{
        //    ImGui::ColorEdit4(id.c_str(), &editable.r);
        //}
        //else if (prop->meta.HasMetaData(MetaTags::Clamp))
        //{
        //    float2 minmax = prop->meta.GetAsFloat2(MetaTags::Clamp);
        //    ImGui::DragFloat4(id.c_str(), &editable.x, 1.0f, minmax.x, minmax.y);
        //}
        //else
        {
            ImGui::InputFloat3(id.c_str(), &editable.x, 3);
        }
    }
    
    //////////////////////////////////////////////////////////////////////////
    
    template<> void DisplayEdit<std::string>(const std::string& id, std::string& editable)
    {
        static constexpr const size_t maxBufferSize = 128u;
    
        char buffer[maxBufferSize];
        std::memcpy(buffer, editable.c_str(), glm::min(maxBufferSize, editable.length()));
    
        ImGui::InputText(id.c_str(), buffer, maxBufferSize);
        editable = buffer;
    }
    
    //////////////////////////////////////////////////////////////////////////
    
    template<> void DisplayEdit<Ref<Material>>(const std::string& id, Ref<Material>& editable)
    {
        // display a list of materials...
    
        std::vector<std::string> materials;
        MaterialLibrary::GetMaterialNames(materials);
    
        const std::string path = editable ? editable->GetPath() : "";
        s32 index = std::distance(materials.begin(), std::find(materials.begin(), materials.end(), path));
        const bool changed = ImGui::Combo(id.c_str(), &index, Helper::ImGuiStringGetter, &materials, materials.size());
    
        if (changed)
        {
            editable = MaterialLibrary::GetMaterial(materials.at(index));
        }
    }
    
    //////////////////////////////////////////////////////////////////////////
    
    template<> void DisplayEdit<Ref<Mesh>>(const std::string& id, Ref<Mesh>& editable)
    {
        // display a list of meshes...
    
        std::vector<std::string> meshes;
        MeshLibrary::GetMeshNames(meshes);
    
        const std::string name = editable ? editable->GetFilePath() : "";
        s32 index = std::distance(meshes.begin(), std::find(meshes.begin(), meshes.end(), name));
        const bool changed = ImGui::Combo(id.c_str(), &index, Helper::ImGuiStringGetter, &meshes, meshes.size());
    
        if (changed)
        {
            editable = MeshLibrary::GetMesh(meshes.at(index));
        }
    }
    
    //////////////////////////////////////////////////////////////////////////
    
    template<> void DisplayEdit<Ref<RTTIObject>>(const std::string& id, Ref<RTTIObject>& editable)
    {
        if (editable)
        {
            DisplayEdit<RTTIObject>(id, *editable);
        }
    }

    //////////////////////////////////////////////////////////////////////////

    template<> void DisplayEdit<RTTIObject>(const std::string& id, RTTIObject& editable)
    {
        TypeBase* type = TypeRegister::GetRegisteredType(editable.GetTypeName());
        while (type != nullptr)
        {
            if (type->GetPropertyCount() > 0)
            {
                if (ImGui::TreeNode(type->GetTypeName().c_str()))
                {
                    for (TypeBase::property_iterator it = type->property_begin(); it != type->property_end(); ++it)
                    {
                        it->second->DisplayEdit(&editable);
                    }

                    ImGui::TreePop();
                }
            }

            type = type->GetBaseType();
        }
    }

    //////////////////////////////////////////////////////////////////////////
}