//////////////////////////////////////////////////////////////////////////
//    File        	: DungeonRoom.cpp
//    Created By    : Jack Spink
//    Created On 	: [7/10/2019]
//////////////////////////////////////////////////////////////////////////

#include "Dungeon/DungeonRoom.h"
#include "Dungeon/DungeonCell.h"
#include "ShaderInput.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Utility/FileUtils.h"
#include "Core/Thread/GameThread.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/ApiManager.h"
#include "Render/Platform/Buffer.h"
#include "Render/Platform/VertexArray.h"
#include "Render/Platform/Material.h"
#include "Render/Platform/MaterialParameter.h"
#include "Render/Platform/Renderer.h"
#include "Render/Platform/Texture2D.h"
#include "Render/Utility/TextureLibrary.h"
#include "Render/Utility/MaterialLibrary.h"
#include "Render/Utility/MeshPrimitiveBuilder.h"

//////////////////////////////////////////////////////////////////////////

#include "nlohmann/json.hpp"

//////////////////////////////////////////////////////////////////////////

#include <vector>

//////////////////////////////////////////////////////////////////////////

using json = nlohmann::json;

//////////////////////////////////////////////////////////////////////////

namespace DungeonBuilder
{
    void AddFloorVertices(const s32 x, const s32 z, std::vector<MeshVertex>& vertices, MeshPrimitiveBuilder::FaceFlag const flags = MeshPrimitiveBuilder::AllFaces)
    {
        std::vector<MeshPrimitiveBuilder::PosNorTex> pnt;
        MeshPrimitiveBuilder::CreateBox(float3((f32)x + 0.5f, 0.1f, (f32)z + 0.5f), float3(0.5f, 0.1f, 0.5f), pnt, flags);

        for (MeshPrimitiveBuilder::PosNorTex& vert : pnt)
        {
            vertices.push_back({ vert.p, vert.n, vert.t });
        }
    }

    void AddWallVertices(const s32 x, const s32 z, std::vector<MeshVertex>& vertices, MeshPrimitiveBuilder::FaceFlag const flags = MeshPrimitiveBuilder::AllFaces)
    {
        std::vector<MeshPrimitiveBuilder::PosNorTex> pnt;
        MeshPrimitiveBuilder::CreateBox(float3((f32)x + 0.5f, DungeonCell::HalfHeight, (f32)z + 0.5f), float3(0.5f, DungeonCell::HalfHeight, 0.5f), pnt, flags);

        for (MeshPrimitiveBuilder::PosNorTex& vert : pnt)
        {
            vertices.push_back({ vert.p, vert.n, vert.t });
        }
    }
}

//////////////////////////////////////////////////////////////////////////

DungeonRoom::DungeonRoom()
    : meshLoaded(false)
{
    auto cellInitialiser = [](const s32 x, const s32 z, DungeonCell*& cell) -> void
    {
        cell = new DungeonCell(x, z);
        cell->SetType(DungeonCell::Floor);
    };

    ForEachCell(cellInitialiser);
}

//////////////////////////////////////////////////////////////////////////

DungeonRoom::~DungeonRoom()
{
    auto cellDeinitialiser = [](const s32 x, const s32 z, DungeonCell*& cell) -> void
    {
        if (cell != nullptr)
        {
            delete cell;
            cell = nullptr;
        }
    };

    ForEachCell(cellDeinitialiser);
}

//////////////////////////////////////////////////////////////////////////

void DungeonRoom::LoadLayoutFromFile(const std::string& layoutfilepath)
{
    auto cellInitialiser = [](const s32 x, const s32 z, DungeonCell*& cell) -> void
    {
        if (cell == nullptr)
            cell = new DungeonCell(x, z);

        cell->SetType(DungeonCell::Floor);

        if (x == 0 || x == Width - 1)
            cell->SetType(DungeonCell::Wall);

        if (z == 0 || z == Depth - 1)
            cell->SetType(DungeonCell::Wall);
    };

    ForEachCell(cellInitialiser);

    std::optional<std::string> jsonString = File::LoadAsString(layoutfilepath);
    if (jsonString.has_value())
    {
        json root = json::parse(jsonString.value());
        auto exits = root["exits"];
        auto layout = root["layout"];

        if (exits.value<bool>("left", false))
        {
            GetCell(Width - 1, 7)->SetType(DungeonCell::Floor);
            GetCell(Width - 1, 8)->SetType(DungeonCell::Floor);
        }

        if (exits.value<bool>("right", false))
        {
            GetCell(0, 7)->SetType(DungeonCell::Floor);
            GetCell(0, 8)->SetType(DungeonCell::Floor);
        }

        if (exits.value<bool>("front", false))
        {
            GetCell(7, Depth - 1)->SetType(DungeonCell::Floor);
            GetCell(8, Depth - 1)->SetType(DungeonCell::Floor);
        }

        if (exits.value<bool>("back", false))
        {
            GetCell(7, 0)->SetType(DungeonCell::Floor);
            GetCell(8, 0)->SetType(DungeonCell::Floor);
        }

        for (auto tile : layout)
        {
            const s32 x = tile.value<s32>("x", -1);
            const s32 z = tile.value<s32>("z", -1);
            const std::string type = tile.value("type", "error");

            if (x == -1 || z == -1 || type == "error")
            {
                printf("error reading json!\n");
                continue;
            }

            DungeonCell* cell = GetCell(x, z);
            if (cell != nullptr)
            {
                cell->SetType(DungeonCell::Wall);
            }
        }
    }

    meshLoaded = false;
    GetGameThread()->PushThreadTask(this, &DungeonRoom::RenderThread_CreateMesh);
}

//////////////////////////////////////////////////////////////////////////

DungeonCell* DungeonRoom::GetCell(const s32 x, const s32 z)
{
    if (x < 0 || x >= Width)
        return nullptr;

    if (z < 0 || z >= Depth)
        return nullptr;

    return cells.at(x + z * Width);
}

//////////////////////////////////////////////////////////////////////////

void DungeonRoom::OnConstruct()
{
    Super::OnConstruct();

    GetGameThread()->PushThreadTask(this, &DungeonRoom::RenderThread_LoadRenderResources);
}

//////////////////////////////////////////////////////////////////////////

void DungeonRoom::OnRender(RenderPassType::Enum pass, Ref<Material> overrideMaterial/* = nullptr*/)
{
    Ref<Material> chosenMaterial = overrideMaterial != nullptr ? overrideMaterial : material;
    if (chosenMaterial != nullptr && meshLoaded)
    {
        Renderer::Submit(chosenMaterial, floor);
        Renderer::Submit(chosenMaterial, walls);
    }
}

//////////////////////////////////////////////////////////////////////////

void DungeonRoom::RenderThread_LoadRenderResources()
{
    material = MaterialLibrary::GetMaterial("mesh-lit-tex");

    floorTexture = TextureLibrary::GetTexture(EngineTextureId::White);
    wallTexture = TextureLibrary::GetTexture(EngineTextureId::White);
    wallTexture->SetTextureParameter(TextureParameterType::WrapMode, TextureParameterValue::Repeat);
}

//////////////////////////////////////////////////////////////////////////

void DungeonRoom::RenderThread_CreateMesh()
{
    std::vector<MeshVertex> floorVertices;
    std::vector<MeshVertex> wallVertices;

    for (DungeonCell* cell : cells)
    {
        DungeonCell* left   = GetCell(cell->GetX() + 1, cell->GetZ() + 0);
        DungeonCell* right  = GetCell(cell->GetX() - 1, cell->GetZ() + 0);
        DungeonCell* front  = GetCell(cell->GetX() + 0, cell->GetZ() + 1);
        DungeonCell* back   = GetCell(cell->GetX() + 0, cell->GetZ() - 1);

        MeshPrimitiveBuilder::FaceFlag flags = MeshPrimitiveBuilder::NoFaces;
        flags |= MeshPrimitiveBuilder::TopFace;
        flags |= MeshPrimitiveBuilder::BottomFace;

        switch (cell->GetType())
        {
            case DungeonCell::Floor:
            {
                if (left == nullptr)
                {
                    flags |= MeshPrimitiveBuilder::LeftFace;
                }

                if (right == nullptr)
                {
                    flags |= MeshPrimitiveBuilder::RightFace;
                }

                if (front == nullptr)
                {
                    flags |= MeshPrimitiveBuilder::FrontFace;
                }

                if (back == nullptr)
                {
                    flags |= MeshPrimitiveBuilder::BackFace;
                }

                DungeonBuilder::AddFloorVertices(cell->GetX(), cell->GetZ(), floorVertices, flags);
                break;
            }

            case DungeonCell::Wall:
            {
                if (left == nullptr || left->GetType() == DungeonCell::Floor)
                {
                    flags |= MeshPrimitiveBuilder::LeftFace;
                }

                if (right == nullptr || right->GetType() == DungeonCell::Floor)
                {
                    flags |= MeshPrimitiveBuilder::RightFace;
                }

                if (front == nullptr || front->GetType() == DungeonCell::Floor)
                {
                    flags |= MeshPrimitiveBuilder::FrontFace;
                }

                if (back == nullptr || back->GetType() == DungeonCell::Floor)
                {
                    flags |= MeshPrimitiveBuilder::BackFace;
                }

                DungeonBuilder::AddWallVertices(cell->GetX(), cell->GetZ(), wallVertices, flags);
                break;
            }
        }
    }

    MeshVertex *pWallVerts = wallVertices.size() > 0 ? &wallVertices.at(0) : nullptr;

    Ref<VertexBuffer> floorBuffer = GetApiManager()->CreateVertexBuffer(floorVertices.size(), sizeof(MeshVertex), &floorVertices.at(0));
    Ref<VertexBuffer> wallBuffer = GetApiManager()->CreateVertexBuffer(wallVertices.size(), sizeof(MeshVertex), pWallVerts);

    floorBuffer->SetLayout(MeshVertex::layout);
    wallBuffer->SetLayout(MeshVertex::layout);

    floor = GetApiManager()->CreateVertexArray();
    walls = GetApiManager()->CreateVertexArray();

    floor->AddBuffer(floorBuffer);
    walls->AddBuffer(wallBuffer);
    meshLoaded = true;
}

//////////////////////////////////////////////////////////////////////////
