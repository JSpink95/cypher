//////////////////////////////////////////////////////////////////////////
//    File        	: MaterialLibrary.cpp
//    Created By    : Jack Spink
//    Created On 	: [18/10/2019]
//////////////////////////////////////////////////////////////////////////

#include "Render/Utility/MaterialLibrary.h"
#include "Render/Platform/Material.h"

//////////////////////////////////////////////////////////////////////////

#include "pugixml.hpp"

//////////////////////////////////////////////////////////////////////////

void MaterialLibrary::Initialise()
{
    Ref<MaterialLibrary> library = GetMaterialLibrary();
    if (library)
    {
        library->InitialiseImpl();
    }
}

//////////////////////////////////////////////////////////////////////////

void MaterialLibrary::Reload()
{
    Ref<MaterialLibrary> library = GetMaterialLibrary();
    if (library)
    {
        library->ReloadImpl();
    }
}

//////////////////////////////////////////////////////////////////////////

Ref<Material> MaterialLibrary::RegisterMaterial(const std::string& id, const std::string& materialPath)
{
    Ref<MaterialLibrary> library = GetMaterialLibrary();
    if (library)
    {
        return library->RegisterMaterialImpl(id, materialPath);
    }

    return nullptr;
}

//////////////////////////////////////////////////////////////////////////

Ref<Material> MaterialLibrary::RegisterMaterial(const std::string& id, Ref<Material> material)
{
    Ref<MaterialLibrary> library = GetMaterialLibrary();
    if (library)
    {
        return library->RegisterMaterialImpl(id, material);
    }

    return nullptr;
}

//////////////////////////////////////////////////////////////////////////

Ref<Material> MaterialLibrary::GetMaterial(const std::string& id)
{
    Ref<MaterialLibrary> library = GetMaterialLibrary();
    if (library)
    {
        return library->GetMaterialImpl(id);
    }

    return nullptr;
}

//////////////////////////////////////////////////////////////////////////

void MaterialLibrary::InitialiseImpl()
{
    // hmm
    // Load MaterialAssets.xml

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("assets/MaterialAssets.xml");
    if (result)
    {
        pugi::xml_node root = doc.root().child("materials");
        for (pugi::xml_node materialNode : root)
        {
            const char* id = materialNode.attribute("id").as_string();
            const char* filepath = materialNode.attribute("filepath").as_string();

#ifdef DEBUG
            printf("Loading material('%s', '%s')\n", id, filepath);
#endif

            RegisterMaterialImpl(id, filepath);
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void MaterialLibrary::ReloadImpl()
{

}

//////////////////////////////////////////////////////////////////////////

Ref<Material> MaterialLibrary::RegisterMaterialImpl(const std::string& id, const std::string& materialPath)
{
    Ref<Material> loadedMaterial = Material::LoadFromFile(materialPath);
    loadedMaterials.insert(std::make_pair(id, loadedMaterial));

    return loadedMaterial;
}

//////////////////////////////////////////////////////////////////////////

Ref<Material> MaterialLibrary::RegisterMaterialImpl(const std::string& id, Ref<Material> material)
{
    loadedMaterials.insert(std::make_pair(id, material));
    return material;
}

//////////////////////////////////////////////////////////////////////////

Ref<Material> MaterialLibrary::GetMaterialImpl(const std::string& id)
{
    auto it = loadedMaterials.find(id);
    if (it != loadedMaterials.end())
        return it->second;

    return nullptr;
}

//////////////////////////////////////////////////////////////////////////

void MaterialLibrary::ReloadMaterialImpl(const std::string& id)
{
    auto it = loadedMaterials.find(id);
    if (it != loadedMaterials.end())
    {
        it->second->Reload();
    }
}

//////////////////////////////////////////////////////////////////////////
