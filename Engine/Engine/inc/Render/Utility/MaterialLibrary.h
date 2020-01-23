//////////////////////////////////////////////////////////////////////////
//    File        	: MaterialLibrary.h
//    Created By    : Jack Spink
//    Created On 	: [18/10/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Singleton.h"

//////////////////////////////////////////////////////////////////////////

#include <unordered_map>

//////////////////////////////////////////////////////////////////////////

class Material;

//////////////////////////////////////////////////////////////////////////

class MaterialLibrary : public Singleton<MaterialLibrary>
{
public:
    static void Initialise();
    static void Reload();

    static Ref<Material> RegisterMaterial(const std::string& id, const std::string& materialPath);
    static Ref<Material> RegisterMaterial(const std::string& id, Ref<Material> material);

    static void GetMaterialNames(std::vector<std::string>& materials);
    static Ref<Material> GetMaterial(const std::string& id);

private:
    void InitialiseImpl();
    void ReloadImpl();

    Ref<Material> RegisterMaterialImpl(const std::string& id, const std::string& materialPath);
    Ref<Material> RegisterMaterialImpl(const std::string& id, Ref<Material> material);

    void GetMaterialNamesImpl(std::vector<std::string>& materials);
    Ref<Material> GetMaterialImpl(const std::string& id);

    void ReloadMaterialImpl(const std::string& id);

private:
    std::unordered_map<std::string, Ref<Material>> loadedMaterials;
};

//////////////////////////////////////////////////////////////////////////

static inline Ref<MaterialLibrary> GetMaterialLibrary() { return MaterialLibrary::Get(); }

//////////////////////////////////////////////////////////////////////////
