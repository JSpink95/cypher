//////////////////////////////////////////////////////////////////////////
//    File        	: MeshLibrary.cpp
//    Created By    : Jack Spink
//    Created On 	: [5/12/2019]
//////////////////////////////////////////////////////////////////////////

#include "Render/Utility/MeshLibrary.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Utility/FileVolumeManager.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Utility/ObjMeshLoader.h"
#include "Render/Utility/DebugMeshVertexGenerator.h"

//////////////////////////////////////////////////////////////////////////

#include "pugixml.hpp"

//////////////////////////////////////////////////////////////////////////

#include <algorithm>

//////////////////////////////////////////////////////////////////////////

void MeshLibrary::Initialise()
{
    if (Ref<MeshLibrary> library = GetMeshLibrary())
    {
        library->InitialiseImpl();
    }
}

//////////////////////////////////////////////////////////////////////////

Ref<VertexArray> MeshLibrary::RegisterMesh(const std::string& id, Ref<VertexArray> mesh)
{
    if (Ref<MeshLibrary> library = GetMeshLibrary())
    {
        return library->RegisterMeshImpl(id, mesh);
    }

    return nullptr;
}

//////////////////////////////////////////////////////////////////////////

void MeshLibrary::GetMeshNames(std::vector<std::string>& output)
{
    if (Ref<MeshLibrary> library = GetMeshLibrary())
    {
        library->GetMeshNamesImpl(output);
    }
}

//////////////////////////////////////////////////////////////////////////

Ref<VertexArray> MeshLibrary::GetMesh(const std::string& id)
{
    if (Ref<MeshLibrary> library = GetMeshLibrary())
    {
        return library->GetMeshImpl(id);
    }

    return nullptr;
}

//////////////////////////////////////////////////////////////////////////

void MeshLibrary::InitialiseImpl()
{
    // load engine objects here...
    RegisterMeshImpl("engine:\\mesh\\wireframe-sphere", DebugMeshVertexGenerator::CreateWireframeSphere(1.0f));

    // now load asset meshes
    PathResult meshAssetPath = FileVolumeManager::GetRealPathFromVirtualPath("assets:\\MeshAssets.xml");
    if (meshAssetPath.valid)
    {
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(meshAssetPath.fullpath.c_str());

        if (result)
        {
            pugi::xml_node root = doc.root().child("meshes");
            for (pugi::xml_node meshNode : root)
            {
                const char* filepath = meshNode.attribute("filepath").as_string();
                const bool hasUvs = meshNode.attribute("load_uvs").as_bool(true);
                const bool hasNormals = meshNode.attribute("load_normals").as_bool(true);
                const f32 scale = meshNode.attribute("scale").as_float(1.0f);

#ifdef DEBUG
                printf("Loading mesh('%s')\n", filepath);
#endif

                // #todo - add support for more than obj meshes

                Ref<VertexArray> vertexList = ObjMeshLoader::LoadObjFromFile(filepath, { hasNormals, hasUvs, scale });
                RegisterMeshImpl(filepath, vertexList);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////

Ref<VertexArray> MeshLibrary::RegisterMeshImpl(const std::string& id, Ref<VertexArray> mesh)
{
    meshes.emplace(id, mesh);
    return mesh;
}

//////////////////////////////////////////////////////////////////////////

void MeshLibrary::GetMeshNamesImpl(std::vector<std::string>& output)
{
    std::transform(meshes.begin(), meshes.end(), std::back_inserter(output), [](const std::pair<std::string, Ref<VertexArray>>& it) -> std::string { return it.first; });
}

//////////////////////////////////////////////////////////////////////////

Ref<VertexArray> MeshLibrary::GetMeshImpl(const std::string& id)
{
    auto it = meshes.find(id);
    if (it != meshes.end())
        return it->second;

    return nullptr;
}

//////////////////////////////////////////////////////////////////////////
