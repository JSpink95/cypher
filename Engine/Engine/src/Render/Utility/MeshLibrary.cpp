//////////////////////////////////////////////////////////////////////////
//    File        	: MeshLibrary.cpp
//    Created By    : Jack Spink
//    Created On 	: [5/12/2019]
//////////////////////////////////////////////////////////////////////////

#include "Render/Utility/MeshLibrary.h"
#include "Render/Utility/ObjMeshLoader.h"

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
    // load default objects here...
    RegisterMeshImpl("game:mesh-capsule", ObjMeshLoader::LoadObjFromFile("assets/models/capsule.obj", { true, true, 1.0f }));
    RegisterMeshImpl("game:mesh-box", ObjMeshLoader::LoadObjFromFile("assets/models/box.obj", { true, true, 1.0f }));
    RegisterMeshImpl("game:mesh-plane", ObjMeshLoader::LoadObjFromFile("assets/models/plane.obj", { true, true, 1.0f }));
    RegisterMeshImpl("game:mesh-sphere-fs", ObjMeshLoader::LoadObjFromFile("assets/models/sphere_fs.obj", { true, true, 1.0f }));
    RegisterMeshImpl("game:mesh-sphere-ss", ObjMeshLoader::LoadObjFromFile("assets/models/sphere_ss.obj", { true, true, 1.0f }));
}

//////////////////////////////////////////////////////////////////////////

Ref<VertexArray> MeshLibrary::RegisterMeshImpl(const std::string& id, Ref<VertexArray> mesh)
{
    meshes.emplace(id, mesh);
    return mesh;
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
