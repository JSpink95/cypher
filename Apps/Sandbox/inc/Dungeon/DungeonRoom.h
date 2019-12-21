//////////////////////////////////////////////////////////////////////////
//    File        	: DungeonRoom.h
//    Created By    : Jack Spink
//    Created On 	: [7/10/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Object/GameObject.h"

//////////////////////////////////////////////////////////////////////////

#include <array>

//////////////////////////////////////////////////////////////////////////

class Material;
class VertexArray;
class Texture2D;

//////////////////////////////////////////////////////////////////////////

class DungeonCell;

//////////////////////////////////////////////////////////////////////////

class DungeonRoom: public GameObject
{
    DECLARE_DERIVED_OBJECT(DungeonRoom, GameObject)
public:
    static inline constexpr const s32 Width = 16;
    static inline constexpr const s32 Depth = 16;
    static inline constexpr const s32 ArraySize = Width * Depth;

    using CellArray = std::array<DungeonCell*, ArraySize>;

public:
    DungeonRoom();
    ~DungeonRoom();

    void LoadLayoutFromFile(const std::string& layoutfilepath);

    // 
    // overriden functions
    // 
    
    virtual void OnConstruct() override;
    virtual void OnRender(RenderPassType::Enum pass, Ref<Material> overrideMaterial = nullptr) override;

    // 
    // accessors
    // 

    Ref<Material> GetMaterial() { return material; }
    DungeonCell* GetCell(const s32 x, const s32 z);

private:
    CellArray cells;

    Ref<Material> material;

    bool meshLoaded;
    Ref<VertexArray> floor;
    Ref<VertexArray> walls;

    Ref<Texture2D> floorTexture;
    Ref<Texture2D> wallTexture;

private:

    void RenderThread_LoadRenderResources();
    void RenderThread_CreateMesh();

public:

    // 
    // iterators
    // 

    template<typename TPredicate>
    void ForEachCell(TPredicate predicate)
    {
        for (s32 x = 0; x < Width; ++x)
            for (s32 z = 0; z < Depth; ++z)
                predicate(x, z, cells.at(x + z * Width));
    }

public:

    // 
    // stl compat
    // 

    CellArray::iterator begin() { return cells.begin(); }
    CellArray::iterator end() { return cells.end(); }
    CellArray::const_iterator begin() const { return cells.begin(); }
    CellArray::const_iterator end() const { return cells.end(); }

};

//////////////////////////////////////////////////////////////////////////
