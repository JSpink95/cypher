//////////////////////////////////////////////////////////////////////////
//    File        	: ObjMeshLoader.cpp
//    Created By    : Jack Spink
//    Created On 	: [15/10/2019]
//////////////////////////////////////////////////////////////////////////

#include "Render/Utility/ObjMeshLoader.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Utility/FileVolumeManager.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Buffer.h"
#include "Render/Platform/VertexArray.h"
#include "Render/Platform/ApiManager.h"

//////////////////////////////////////////////////////////////////////////

#include <fstream>

//////////////////////////////////////////////////////////////////////////

struct FaceVertex
{
    s32 p, n, t;
};

//////////////////////////////////////////////////////////////////////////

struct Face
{
    FaceVertex a, b, c;

    inline float3 CalculateFaceNormal(std::vector<float3> const& positions) const
    {
        float3 const& pa = positions.at(a.p);
        float3 const& pb = positions.at(b.p);
        float3 const& pc = positions.at(c.p);

        return glm::cross(glm::normalize(pb - pa), glm::normalize(pc - pa));
    }

    inline FaceVertex& operator[](const s32 i)
    {
        switch (i)
        {
            case 0: return a;
            case 1: return b;
            case 2: return c;
            default: return a;
        }
    }

    inline FaceVertex const& operator[](const s32 i) const
    {
        switch (i)
        {
            case 0: return a;
            case 1: return b;
            case 2: return c;
            default: return a;
        }
    }
};

//////////////////////////////////////////////////////////////////////////

std::vector<std::string> split_string(const std::string& string, const char on = ' ')
{
    std::vector<std::string> result;

    std::string current;
    for (std::string::const_iterator it = string.begin(); it != string.end();)
    {
        if (*it != on)
            current += *it;

        if ((it + 1) == string.end() || *it == on)
        {
            result.push_back(current);
            current = "";
        }

        ++it;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////

Ref<VertexBuffer> CreateBufferFromFaceData(
    const std::vector<Face>& faces, LoadMeshParams const& params,
    const std::vector<float3>& positions, const std::vector<float3>& normals, const std::vector<float2>& texcoords
)
{
    bool const hasTexcoords = texcoords.size() > 0;
    bool const hasNormals = normals.size() > 0;

    if (params.loadNormals && params.loadTexcoords)
    {
        std::vector<ObjVertex_Pos3Nor3Tex2> vertices;
        for (Face const& face : faces)
        {
            for (u32 idx = 0; idx < 3; ++idx)
            {
                ObjVertex_Pos3Nor3Tex2 vertex;
                vertex.position = positions.at(face[idx].p);

                if (hasNormals)
                    vertex.normal = normals.at(face[idx].n);
                else
                    vertex.normal = face.CalculateFaceNormal(positions);

                if (hasTexcoords)
                    vertex.texcoord = texcoords.at(face[idx].t);
                else
                    vertex.texcoord = vec2(0.0f);

                vertices.push_back(vertex);
            }
        }

        Ref<VertexBuffer> buffer = GetApiManager()->CreateVertexBuffer(vertices.size(), sizeof(ObjVertex_Pos3Nor3Tex2), &vertices.at(0));
        buffer->SetLayout(ObjVertex_Pos3Nor3Tex2::layout);

        return buffer;
    }
    else if (params.loadNormals)
    {
        std::vector<ObjVertex_Pos3Nor3> vertices;
        for (Face const& face : faces)
        {
            for (u32 idx = 0; idx < 3; ++idx)
            {
                ObjVertex_Pos3Nor3 vertex;
                vertex.position = positions.at(face[idx].p);

                if (hasNormals)
                    vertex.normal = normals.at(face[idx].n);
                else
                    vertex.normal = face.CalculateFaceNormal(positions);


                vertices.push_back(vertex);
            }
        }

        Ref<VertexBuffer> buffer = GetApiManager()->CreateVertexBuffer(vertices.size(), sizeof(ObjVertex_Pos3Nor3), &vertices.at(0));
        buffer->SetLayout(ObjVertex_Pos3Nor3::layout);

        return buffer;
    }
    else if (params.loadTexcoords)
    {
        std::vector<ObjVertex_Pos3Tex2> vertices;
        for (Face const& face : faces)
        {
            for (u32 idx = 0; idx < 3; ++idx)
            {
                ObjVertex_Pos3Tex2 vertex;
                vertex.position = positions.at(face[idx].p);

                if (hasTexcoords)
                    vertex.texcoord = texcoords.at(face[idx].t);
                else
                    vertex.texcoord = vec2(0.0f);

                vertices.push_back(vertex);
            }
        }

        Ref<VertexBuffer> buffer = GetApiManager()->CreateVertexBuffer(vertices.size(), sizeof(ObjVertex_Pos3Tex2), &vertices.at(0));
        buffer->SetLayout(ObjVertex_Pos3Tex2::layout);

        return buffer;
    }
    else
    {
        std::vector<ObjVertex_Pos3> vertices;
        for (Face const& face : faces)
        {
            for (u32 idx = 0; idx < 3; ++idx)
            {
                ObjVertex_Pos3 vertex;
                vertex.position = positions.at(face[idx].p);

                vertices.push_back(vertex);
            }
        }

        Ref<VertexBuffer> buffer = GetApiManager()->CreateVertexBuffer(vertices.size(), sizeof(ObjVertex_Pos3), &vertices.at(0));
        buffer->SetLayout(ObjVertex_Pos3::layout);

        return buffer;
    }
}

//////////////////////////////////////////////////////////////////////////

Ref<VertexArray> ObjMeshLoader::LoadObjFromFile(const std::string& filepath, LoadMeshParams const& params/* = { true, true, 1.0f }*/)
{
    const std::string convertedFilePath = FileVolumeManager::GetRealPathFromVirtualPath(filepath).fullpath;
    std::ifstream file(convertedFilePath.c_str());
    if (file.fail())
    {
        return nullptr;
    }

    auto convertStringsToFloat2 = [](const std::vector<std::string>& strings) -> float2
    {
        assert(strings.size() >= 2);
        return float2(std::stof(strings.at(0)), std::stof(strings.at(1)));
    };

    auto convertStringsToFloat3 = [](const std::vector<std::string>& strings) -> float3
    {
        assert(strings.size() >= 3);
        return float3(std::stof(strings.at(0)), std::stof(strings.at(1)), std::stof(strings.at(2)));
    };

    std::vector<float3> positions;
    std::vector<float3> normals;
    std::vector<float2> texcoords;

    std::vector<Face> faces;

    std::string line;
    while (std::getline(file, line))
    {
        if (line.length() == 0 || line.at(0) == '#')
            continue;

        if (line.at(0) == 'v')
        {
            char const type = line.at(1);

            std::string data = line.substr(line.find_first_of(' ') + 1);
            std::vector<std::string> numbers = split_string(data, ' ');

            switch (type)
            {
                case ' ':
                {
                    // load position + apply the scale from the load params
                    positions.push_back(convertStringsToFloat3(numbers) * params.modelScale);
                    break;
                }

                case 'n':
                {
                    // load normals
                    if (params.loadNormals)
                    {
                        normals.push_back(convertStringsToFloat3(numbers));
                    }
                    break;
                }

                case 't':
                {
                    // load texcoord
                    if (params.loadTexcoords)
                    {
                        texcoords.push_back(convertStringsToFloat2(numbers));
                    }
                    break;
                }

                default:
                    printf("%c is not a supported obj vertex type!\n", type);
                    break;
            }
        }
        else if (line.at(0) == 'f')
        {
            // read faces
            // in obj files, faces start at index 1 (v_v)

            Face face;

            std::string data = line.substr(line.find_first_of(' ') + 1);
            std::vector<std::string> faceVertices = split_string(data, ' ');

            // uh-oh, we have a quad! Make it into two triangles!
            if (faceVertices.size() == 4)
            {
                std::vector<std::string> faceVertsCopy = faceVertices;
                faceVertices.clear();

                faceVertices.push_back(faceVertsCopy.at(0)); faceVertices.push_back(faceVertsCopy.at(1)); faceVertices.push_back(faceVertsCopy.at(2));
                faceVertices.push_back(faceVertsCopy.at(1)); faceVertices.push_back(faceVertsCopy.at(2)); faceVertices.push_back(faceVertsCopy.at(3));
            }

            s32 faceVertexIndex = 0;
            for (std::string const& faceVertex : faceVertices)
            {
                std::vector<std::string> faceVertexData = split_string(faceVertex, '/');

                s32 const positionVertexIndex = std::stoi(faceVertexData.at(0)) - 1;
                face[faceVertexIndex].p = positionVertexIndex;

                if (params.loadTexcoords && faceVertexData.size() >= 2 && faceVertexData.at(1) != "")
                {
                    s32 const texcoordVertexIndex = std::stoi(faceVertexData.at(1)) - 1;
                    face[faceVertexIndex].t = texcoordVertexIndex;
                }

                if (params.loadNormals && faceVertexData.size() >= 3 && faceVertexData.at(2) != "")
                {
                    s32 const normalVertexIndex = std::stoi(faceVertexData.at(2)) - 1;
                    face[faceVertexIndex].n = normalVertexIndex;
                }

                ++faceVertexIndex;
                faceVertexIndex %= 3;
            }

            faces.push_back(face);
        }
    }

    file.close();

    Ref<VertexBuffer> buffer = CreateBufferFromFaceData(faces, params, positions, normals, texcoords);
    Ref<VertexArray> mesh = GetApiManager()->CreateVertexArray();
    mesh->AddBuffer(buffer);

    return mesh;
}

//////////////////////////////////////////////////////////////////////////

MtlFileResult MtlFileLoader::LoadMtlFromFile(const std::string& filepath)
{
    MtlFileResult result;

    //std::ifstream file(filepath.c_str());
    //if (file.fail())
    //{
    //    return result;
    //}

    //file.close();

    return result;
}

//////////////////////////////////////////////////////////////////////////
