//////////////////////////////////////////////////////////////////////////
//    File        	: ObjMeshLoader.h
//    Created By    : Jack Spink
//    Created On 	: [15/10/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"
#include "Render/Platform/Buffer.h"

//////////////////////////////////////////////////////////////////////////

#include <string>

//////////////////////////////////////////////////////////////////////////

class VertexArray;

//////////////////////////////////////////////////////////////////////////

struct LoadMeshParams
{
    bool loadNormals = true;
    bool loadTexcoords = true;
    f32 modelScale = 1.0f;
};

//////////////////////////////////////////////////////////////////////////

struct ObjVertex_Pos3
{
    static inline VertexBufferLayout layout = {
        { "aPosition", ShaderData::Float3 },
    };

    float3 position;
};

struct ObjVertex_Pos3Nor3
{
    static inline VertexBufferLayout layout = {
        { "aPosition", ShaderData::Float3 },
        { "aNormal", ShaderData::Float3 },
    };

    float3 position, normal;
};

struct ObjVertex_Pos3Tex2
{
    static inline VertexBufferLayout layout = {
        { "aPosition", ShaderData::Float3 },
        { "aTexcoord", ShaderData::Float2 },
    };

    float3 position;
    float2 texcoord;
};

struct ObjVertex_Pos3Nor3Tex2
{
    static inline VertexBufferLayout layout = {
        { "aPosition", ShaderData::Float3 },
        { "aNormal", ShaderData::Float3 },
        { "aTexcoord", ShaderData::Float2 },
    };

    float3 position, normal;
    float2 texcoord;
};

//////////////////////////////////////////////////////////////////////////

struct MtlFileResult
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

//////////////////////////////////////////////////////////////////////////

class ObjMeshLoader
{
public:
    static Ref<VertexArray> LoadObjFromFile(const std::string& filepath, LoadMeshParams const& params = { true, true, 1.0f });
};

//////////////////////////////////////////////////////////////////////////

class MtlFileLoader
{
public:
    static MtlFileResult LoadMtlFromFile(const std::string& filepath);
};

//////////////////////////////////////////////////////////////////////////
