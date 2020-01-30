//////////////////////////////////////////////////////////////////////////
//    File        	: Mesh.h
//    Created By    : Jack Spink
//    Created On 	: [30/01/2020]
//////////////////////////////////////////////////////////////////////////

#include "Render/Mesh.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Renderer.h"
#include "Render/Utility/ObjMeshLoader.h"

//////////////////////////////////////////////////////////////////////////

Mesh::Mesh(const std::string& filepath)
{
    LoadFromFile(filepath);
}

//////////////////////////////////////////////////////////////////////////

Mesh::Mesh(const std::string& id, Ref<VertexArray> vertexlist)
{
    SubMesh submesh;
    submesh.vao = vertexlist;

    filepath = id;
    meshes.emplace(id, submesh);
}

//////////////////////////////////////////////////////////////////////////

bool Mesh::LoadFromFile(const std::string& fp)
{
    filepath = fp;

    // #todo - Check for file type here and use the appropriate loader.
    //         For now, everything is an obj.

    std::vector<ObjMeshObject> vaos = ObjMeshLoader::LoadObjFromFile(filepath, { true, true, 1.0f });

    if (vaos.size() == 0)
    {
        return false;
    }

    // now add all these meshes into our mesh list

    for (ObjMeshObject& objSubMesh : vaos)
    {
        SubMesh submesh;
        submesh.localTransform = fmat4(1.0f);
        submesh.vao = objSubMesh.vao;

        meshes.emplace(objSubMesh.id, submesh);
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////

Mesh::iterator Mesh::FindSubMesh(const std::string& id)
{
    return meshes.find(id);
}

//////////////////////////////////////////////////////////////////////////

Mesh::const_iterator Mesh::FindSubMesh(const std::string& id) const
{
    return meshes.find(id);
}

//////////////////////////////////////////////////////////////////////////

void Mesh::Render(Ref<Material> material, const fmat4& objectTransform)
{
    for (auto& it : meshes)
    {
        SubMesh& submesh = it.second;
        if (submesh.vao != nullptr)
        {
            Renderer::Submit(material, submesh.vao, objectTransform * submesh.localTransform);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
