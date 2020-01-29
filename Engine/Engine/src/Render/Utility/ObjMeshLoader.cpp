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

    inline float3 CalculateFaceNormal(const std::vector<float3>& positions) const
    {
        const float3& pa = positions.at(a.p);
        const float3& pb = positions.at(b.p);
        const float3& pc = positions.at(c.p);

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

struct ObjectGroup
{
public:
    ObjectGroup(const std::string& inName)
        : name(inName)
    {}

public:
    const std::string name;

public:
    s32 positionStartOffset = 0u;
    s32 normalStartOffset = 0u;
    s32 texcoordStartOffset = 0u;

    std::vector<float3> positions, normals;
    std::vector<float2> texcoords;
    std::vector<Face> faces;
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

Ref<VertexBuffer> CreateBufferFromFaceData(const ObjectGroup& object, const LoadMeshParams& params)
{
    const bool hasTexcoords = object.texcoords.size() > 0;
    const bool hasNormals = object.normals.size() > 0;

    if (params.loadNormals && params.loadTexcoords)
    {
        std::vector<ObjVertex_Pos3Nor3Tex2> vertices;
        for (const Face& face : object.faces)
        {
            for (u32 idx = 0; idx < 3; ++idx)
            {
                ObjVertex_Pos3Nor3Tex2 vertex;
                vertex.position = object.positions.at(face[idx].p);

                if (hasNormals)
                    vertex.normal = object.normals.at(face[idx].n);
                else
                    vertex.normal = face.CalculateFaceNormal(object.positions);

                if (hasTexcoords)
                    vertex.texcoord = object.texcoords.at(face[idx].t);
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
        for (const Face& face : object.faces)
        {
            for (u32 idx = 0; idx < 3; ++idx)
            {
                ObjVertex_Pos3Nor3 vertex;
                vertex.position = object.positions.at(face[idx].p);

                if (hasNormals)
                    vertex.normal = object.normals.at(face[idx].n);
                else
                    vertex.normal = face.CalculateFaceNormal(object.positions);

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
        for (const Face& face : object.faces)
        {
            for (u32 idx = 0; idx < 3; ++idx)
            {
                ObjVertex_Pos3Tex2 vertex;
                vertex.position = object.positions.at(face[idx].p);

                if (hasTexcoords)
                    vertex.texcoord = object.texcoords.at(face[idx].t);
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
        for (const Face& face : object.faces)
        {
            for (u32 idx = 0; idx < 3; ++idx)
            {
                ObjVertex_Pos3 vertex;
                vertex.position = object.positions.at(face[idx].p);

                vertices.push_back(vertex);
            }
        }

        Ref<VertexBuffer> buffer = GetApiManager()->CreateVertexBuffer(vertices.size(), sizeof(ObjVertex_Pos3), &vertices.at(0));
        buffer->SetLayout(ObjVertex_Pos3::layout);

        return buffer;
    }
}

//////////////////////////////////////////////////////////////////////////

Ref<VertexArray> ObjMeshLoader::LoadObjFromFile(const std::string& filepath, const LoadMeshParams& params/* = { true, true, 1.0f }*/)
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

    std::string currentObjectId = "";
    std::unordered_map<std::string, ObjectGroup> objects;

    s32 maxPositionIndex = 0;
    s32 maxNormalIndex = 0;
    s32 maxTexcoordIndex = 0;

    std::string line;
    while (std::getline(file, line))
    {
        if (line.length() == 0 || line.at(0) == '#')
            continue;

        if (line.at(0) == 'v')
        {
            if (currentObjectId == "")
            {
                currentObjectId = "root";
                objects.emplace(currentObjectId, ObjectGroup(currentObjectId));
            }

            ObjectGroup& group = objects.at(currentObjectId);
            char const type = line.at(1);

            std::string data = line.substr(line.find_first_of(' ') + 1);
            std::vector<std::string> numbers = split_string(data, ' ');

            switch (type)
            {
                case ' ':
                {
                    // load position + apply the scale from the load params
                    group.positions.push_back(convertStringsToFloat3(numbers) * params.modelScale);
                    break;
                }

                case 'n':
                {
                    // load normals
                    if (params.loadNormals)
                    {
                        group.normals.push_back(convertStringsToFloat3(numbers));
                    }
                    break;
                }

                case 't':
                {
                    // load texcoord
                    if (params.loadTexcoords)
                    {
                        group.texcoords.push_back(convertStringsToFloat2(numbers));
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

            ObjectGroup& group = objects.at(currentObjectId);

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
            for (const std::string& faceVertex : faceVertices)
            {
                std::vector<std::string> faceVertexData = split_string(faceVertex, '/');

                const s32 positionVertexIndex = std::stoi(faceVertexData.at(0));
                maxPositionIndex = glm::max(maxPositionIndex, positionVertexIndex);

                face[faceVertexIndex].p = positionVertexIndex - group.positionStartOffset - 1;

                if (params.loadTexcoords && faceVertexData.size() >= 2 && faceVertexData.at(1) != "")
                {
                    const s32 texcoordVertexIndex = std::stoi(faceVertexData.at(1));
                    maxTexcoordIndex = glm::max(maxTexcoordIndex, texcoordVertexIndex);

                    face[faceVertexIndex].t = texcoordVertexIndex - group.texcoordStartOffset - 1;
                }

                if (params.loadNormals && faceVertexData.size() >= 3 && faceVertexData.at(2) != "")
                {
                    const s32 normalVertexIndex = std::stoi(faceVertexData.at(2));
                    maxNormalIndex = glm::max(maxNormalIndex, normalVertexIndex);

                    face[faceVertexIndex].n = normalVertexIndex - group.normalStartOffset - 1;
                }

                ++faceVertexIndex;
                faceVertexIndex %= 3;
            }

            group.faces.push_back(face);
        }
        else if (line.at(0) == 'o')
        {
            // new object in the mesh, we should treat this as a new vertex buffer
            currentObjectId = line.substr(line.find_first_of(' ') + 1);

            ObjectGroup object = ObjectGroup(currentObjectId);
            object.positionStartOffset = maxPositionIndex;
            object.normalStartOffset = maxNormalIndex;
            object.texcoordStartOffset = maxTexcoordIndex;

            objects.emplace(currentObjectId, object);
        }
    }

    file.close();

    Ref<VertexArray> mesh = GetApiManager()->CreateVertexArray();

    for (auto& it : objects)
    {
        ObjectGroup& object = it.second;
        Ref<VertexBuffer> buffer = CreateBufferFromFaceData(object, params);
        mesh->AddBuffer(buffer);
    }

    return mesh;
}

//////////////////////////////////////////////////////////////////////////

MtlFileResult MtlFileLoader::LoadMtlFromFile(const std::string& filepath)
{
    MtlFileResult result;

    return result;
}

//////////////////////////////////////////////////////////////////////////
