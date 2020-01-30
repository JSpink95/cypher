//////////////////////////////////////////////////////////////////////////
//    File        	: Mesh.h
//    Created By    : Jack Spink
//    Created On 	: [30/01/2020]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"

//////////////////////////////////////////////////////////////////////////

class Material;
class VertexArray;

//////////////////////////////////////////////////////////////////////////

class SubMesh
{
public:
    fmat4 localTransform = fmat4(1.0f);
    Ref<VertexArray> vao = nullptr;
};

//////////////////////////////////////////////////////////////////////////

class Mesh
{
public:
    using iterator = HashMap<std::string, SubMesh>::iterator;
    using const_iterator = HashMap<std::string, SubMesh>::const_iterator;

public:
    Mesh(const std::string& filepath);
    Mesh(const std::string& id, Ref<VertexArray> vertexlist);

    inline const std::string& GetFilePath() const { return filepath; }

public:
    bool LoadFromFile(const std::string& filepath);

public:
    iterator FindSubMesh(const std::string& id);
    const_iterator FindSubMesh(const std::string& id) const;

public:
    void Render(Ref<Material> material, const fmat4& objectTransform);

public:
    inline iterator begin() { return meshes.begin(); }
    inline const_iterator begin() const { return meshes.begin(); }

    inline iterator end() { return meshes.end(); }
    inline const_iterator end() const { return meshes.end(); }

private:
    std::string filepath;
    HashMap<std::string, SubMesh> meshes;
};

//////////////////////////////////////////////////////////////////////////
