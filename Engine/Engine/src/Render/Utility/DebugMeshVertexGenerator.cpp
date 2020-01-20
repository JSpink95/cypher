//////////////////////////////////////////////////////////////////////////
//    File        	: DebugMeshVertexGenerator.cpp
//    Created By    : Jack Spink
//    Created On 	: [20/01/2020]
//////////////////////////////////////////////////////////////////////////

#include "Render/Utility/DebugMeshVertexGenerator.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/ApiManager.h"
#include "Render/Platform/VertexArray.h"
#include "Render/Platform/Buffer.h"

//////////////////////////////////////////////////////////////////////////

struct WireframeVertex
{
    static inline VertexBufferLayout layout = {
        { "aPosition", ShaderData::Float3 }
    };

    float3 position;
};

//////////////////////////////////////////////////////////////////////////

Ref<VertexArray> DebugMeshVertexGenerator::CreateWireframeBox(const float3& hs)
{
    WireframeVertex vertices[16] =
    {
        { float3(-hs.x, -hs.y, -hs.z) }, { float3( hs.x, -hs.y, -hs.z) },
        { float3(-hs.x,  hs.y, -hs.z) }, { float3( hs.x,  hs.y, -hs.z) },
        { float3(-hs.x, -hs.y,  hs.z) }, { float3( hs.x, -hs.y,  hs.z) },
        { float3(-hs.x,  hs.y,  hs.z) }, { float3( hs.x,  hs.y,  hs.z) },

        { float3(-hs.x, -hs.y, -hs.z) }, { float3(-hs.x, -hs.y,  hs.z) },
        { float3(-hs.x,  hs.y, -hs.z) }, { float3(-hs.x,  hs.y,  hs.z) },
        { float3( hs.x, -hs.y, -hs.z) }, { float3( hs.x, -hs.y,  hs.z) },
        { float3( hs.x,  hs.y, -hs.z) }, { float3( hs.x,  hs.y,  hs.z) },
    };

    Ref<VertexBuffer> buffer = GetApiManager()->CreateVertexBuffer(8u, sizeof(WireframeVertex), vertices);
    buffer->SetLayout(WireframeVertex::layout);

    Ref<VertexArray> result = GetApiManager()->CreateVertexArray();
    result->AddBuffer(buffer);

    return result;
}

//////////////////////////////////////////////////////////////////////////

Ref<VertexArray> DebugMeshVertexGenerator::CreateWireframeSphere(const f32 radius)
{
    static constexpr u32 circlePointCount = 32u;

    std::vector<WireframeVertex> vertices;

    for (u32 idx = 0; idx < circlePointCount; ++idx)
    {
        const f32 percentA = (f32)idx / (f32)circlePointCount;
        const f32 percentB = (f32)(idx + 1) / (f32)circlePointCount;

        const f32 a = percentA * glm::pi<f32>() * 2.0f;
        const f32 b = percentB * glm::pi<f32>() * 2.0f;

        // add x axis

        vertices.push_back({ float3(0.0f, glm::cos(a), glm::sin(a)) });
        vertices.push_back({ float3(0.0f, glm::cos(b), glm::sin(b)) });

        // add y axis

        vertices.push_back({ float3(glm::cos(a), 0.0f, glm::sin(a)) });
        vertices.push_back({ float3(glm::cos(b), 0.0f, glm::sin(b)) });

        // add z axis

        vertices.push_back({ float3(glm::cos(a), glm::sin(a), 0.0f) });
        vertices.push_back({ float3(glm::cos(b), glm::sin(b), 0.0f) });

    }

    Ref<VertexBuffer> buffer = GetApiManager()->CreateVertexBuffer(vertices.size(), sizeof(WireframeVertex), &vertices.at(0));
    buffer->SetLayout(WireframeVertex::layout);

    Ref<VertexArray> result = GetApiManager()->CreateVertexArray();
    result->AddBuffer(buffer);

    return result;
}

//////////////////////////////////////////////////////////////////////////
