//////////////////////////////////////////////////////////////////////////
//    File        	: MeshLibrary.cpp
//    Created By    : Jack Spink
//    Created On 	: [5/12/2019]
//////////////////////////////////////////////////////////////////////////

#include "Render/Utility/MeshLibrary.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Utility/Console.h"
#include "Core/Utility/FileVolumeManager.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Mesh.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Utility/ObjMeshLoader.h"
#include "Render/Utility/DebugMeshVertexGenerator.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Buffer.h"
#include "Render/Platform/ApiManager.h"
#include "Render/Platform/VertexArray.h"

//////////////////////////////////////////////////////////////////////////

#include "pugixml.hpp"

//////////////////////////////////////////////////////////////////////////

#include <algorithm>

//////////////////////////////////////////////////////////////////////////

struct PositionVertex
{
    static inline VertexBufferLayout layout = {
        { "aPosition", ShaderData::Float2 }
    };

    float2 position;
};

//////////////////////////////////////////////////////////////////////////

struct MeshGenerator
{
    static Ref<VertexArray> CreateScreenQuad();
};

//////////////////////////////////////////////////////////////////////////

Ref<VertexArray> MeshGenerator::CreateScreenQuad()
{
    PositionVertex vertices[] =
    {
        { float2(-1.0f, -1.0f) }, { float2(1.0f, -1.0f) }, { float2( 1.0f, 1.0f) },
        { float2(-1.0f, -1.0f) }, { float2(1.0f,  1.0f) }, { float2(-1.0f, 1.0f) },
    };

    Ref<VertexBuffer> buffer = GetApiManager()->CreateVertexBuffer(6, sizeof(PositionVertex), vertices);
    buffer->SetLayout(PositionVertex::layout);

    Ref<VertexArray> mesh = GetApiManager()->CreateVertexArray();
    mesh->AddBuffer(buffer);

    return mesh;
}

//////////////////////////////////////////////////////////////////////////

void MeshLibrary::Initialise()
{
    if (Ref<MeshLibrary> library = GetMeshLibrary())
    {
        library->InitialiseImpl();
    }
}

//////////////////////////////////////////////////////////////////////////

Ref<Mesh> MeshLibrary::RegisterMesh(const std::string& id, Ref<Mesh> mesh)
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

Ref<Mesh> MeshLibrary::GetMesh(const std::string& id)
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
    RegisterMeshImpl("engine:\\mesh\\wireframe-sphere", std::make_shared<Mesh>("wireframe-sphere", DebugMeshVertexGenerator::CreateWireframeSphere(1.0f)));
    RegisterMeshImpl("engine:\\mesh\\screen-quad", std::make_shared<Mesh>("screen-quad", MeshGenerator::CreateScreenQuad()));

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

                LOG_INFO("Loading mesh('%s')\n", filepath);

                Ref<Mesh> mesh = std::make_shared<Mesh>(filepath);
                RegisterMeshImpl(filepath, mesh);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////

Ref<Mesh> MeshLibrary::RegisterMeshImpl(const std::string& id, Ref<Mesh> mesh)
{
    meshes.emplace(id, mesh);
    return mesh;
}

//////////////////////////////////////////////////////////////////////////

void MeshLibrary::GetMeshNamesImpl(std::vector<std::string>& output)
{
    std::transform(meshes.begin(), meshes.end(), std::back_inserter(output), [](const std::pair<std::string, Ref<Mesh>>& it) -> std::string { return it.first; });
}

//////////////////////////////////////////////////////////////////////////

Ref<Mesh> MeshLibrary::GetMeshImpl(const std::string& id)
{
    auto it = meshes.find(id);
    if (it != meshes.end())
    {
        return it->second;
    }

    return nullptr;
}

//////////////////////////////////////////////////////////////////////////
