//////////////////////////////////////////////////////////////////////////
//    File        	: MeshLibrary.h
//    Created By    : Jack Spink
//    Created On 	: [5/12/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Singleton.h"

//////////////////////////////////////////////////////////////////////////

class VertexArray;

//////////////////////////////////////////////////////////////////////////

class MeshLibrary : public Singleton<MeshLibrary>
{
public:
    static void Initialise();

    static Ref<VertexArray> RegisterMesh(const std::string& id, Ref<VertexArray> mesh);

    static void GetMeshNames(std::vector<std::string>& output);
    static Ref<VertexArray> GetMesh(const std::string& id);

private:
    void InitialiseImpl();

    Ref<VertexArray> RegisterMeshImpl(const std::string& id, Ref<VertexArray> mesh);

    void GetMeshNamesImpl(std::vector<std::string>& output);
    Ref<VertexArray> GetMeshImpl(const std::string& id);

private:
    using mesh_map_iterator = std::unordered_map<std::string, Ref<VertexArray>>::iterator;
    std::unordered_map<std::string, Ref<VertexArray>> meshes;
};

//////////////////////////////////////////////////////////////////////////

static inline Ref<MeshLibrary> GetMeshLibrary() { return MeshLibrary::Get(); }

//////////////////////////////////////////////////////////////////////////
