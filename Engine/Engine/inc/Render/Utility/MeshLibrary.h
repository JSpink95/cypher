//////////////////////////////////////////////////////////////////////////
//    File        	: MeshLibrary.h
//    Created By    : Jack Spink
//    Created On 	: [5/12/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Singleton.h"

//////////////////////////////////////////////////////////////////////////

class Mesh;

//////////////////////////////////////////////////////////////////////////

class MeshLibrary : public Singleton<MeshLibrary>
{
public:
    static void Initialise();

    static Ref<Mesh> RegisterMesh(const std::string& id, Ref<Mesh> mesh);

    static void GetMeshNames(std::vector<std::string>& output);
    static Ref<Mesh> GetMesh(const std::string& id);

private:
    void InitialiseImpl();

    Ref<Mesh> RegisterMeshImpl(const std::string& id, Ref<Mesh> mesh);

    void GetMeshNamesImpl(std::vector<std::string>& output);
    Ref<Mesh> GetMeshImpl(const std::string& id);

private:
    using mesh_map_iterator = HashMap<std::string, Ref<Mesh>>::iterator;
    HashMap<std::string, Ref<Mesh>> meshes;
};

//////////////////////////////////////////////////////////////////////////

static inline Ref<MeshLibrary> GetMeshLibrary() { return MeshLibrary::Get(); }

//////////////////////////////////////////////////////////////////////////
