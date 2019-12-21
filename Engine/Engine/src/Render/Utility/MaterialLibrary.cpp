//////////////////////////////////////////////////////////////////////////
//    File        	: MaterialLibrary.cpp
//    Created By    : Jack Spink
//    Created On 	: [18/10/2019]
//////////////////////////////////////////////////////////////////////////

#include "Render/Utility/MaterialLibrary.h"
#include "Render/Platform/Material.h"

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

    RegisterMaterialImpl("game:mesh-lit", "assets/materials/mesh-lit.xml");
    RegisterMaterialImpl("game:mesh-lit-tex", "assets/materials/mesh-lit-tex.xml");
    RegisterMaterialImpl("game:pp-image-render", "assets/materials/pp-image-render.xml");
    RegisterMaterialImpl("game:pp-overlay", "assets/materials/pp-overlay.xml");
    RegisterMaterialImpl("game:pp-ssao", "assets/materials/pp-ssao.xml");
    RegisterMaterialImpl("game:pp-blur", "assets/materials/pp-blur.xml");
    RegisterMaterialImpl("game:pp-ssl", "assets/materials/pp-ssl.xml");
    RegisterMaterialImpl("game:shadow-pass", "assets/materials/shadow-pass.xml");
    RegisterMaterialImpl("game:particle-default", "assets/materials/particle-pos-size.xml");

    // load "instances"
    RegisterMaterialImpl("game:mesh-lit-tex:error", "assets/materials/mesh-lit-tex-error.xml");
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
