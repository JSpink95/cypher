//////////////////////////////////////////////////////////////////////////
//    File        	: Material.cpp
//    Created By    : Jack Spink
//    Created On 	: [23/9/2019]
//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Material.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Utility/FileVolumeManager.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Renderer.h"
#include "Render/Platform/CoreMaterialParameterBlocks.h"
#include "Render/Platform/MaterialParameter.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/ApiManager.h"
#include "Render/Platform/Shader.h"
#include "Render/Utility/TextureLibrary.h"
#include "Render/Utility/MaterialLibrary.h"

//////////////////////////////////////////////////////////////////////////

#include "pugixml.hpp"

//////////////////////////////////////////////////////////////////////////

namespace MaterialParameterType
{
    enum Enum
    {
        Float1, Float2, Float3, Float4,
        Texture2D,
        PerFrameBlock, PerObjectBlock, TimeBlock, CustomBlock,
        Max
    };
}

//////////////////////////////////////////////////////////////////////////

static const std::unordered_map<std::string, MaterialParameterType::Enum> MaterialTypeMap =
{
    // floats
    std::make_pair("float1", MaterialParameterType::Float1),
    std::make_pair("float2", MaterialParameterType::Float2),
    std::make_pair("float3", MaterialParameterType::Float3),
    std::make_pair("float4", MaterialParameterType::Float4),

    // textures
    std::make_pair("texture2D", MaterialParameterType::Texture2D),

    // blocks
    std::make_pair("materialblock-perframe", MaterialParameterType::PerFrameBlock),
    std::make_pair("materialblock-perobject", MaterialParameterType::PerObjectBlock),
    std::make_pair("materialblock-time", MaterialParameterType::TimeBlock),
    std::make_pair("materialblock-custom", MaterialParameterType::CustomBlock),
};

static const std::unordered_map<std::string, size_t> MaterialTypeMemorySize =
{
    // floats
    std::make_pair("float1", sizeof(f32) * 1),
    std::make_pair("float2", sizeof(f32) * 2),
    std::make_pair("float3", sizeof(f32) * 3),
    std::make_pair("float4", sizeof(f32) * 4),

    // blocks
    std::make_pair("materialblock-perframe", sizeof(MaterialFrameProperties)),
    std::make_pair("materialblock-perobject", sizeof(MaterialObjectProperties)),
    std::make_pair("materialblock-time", sizeof(MaterialTimeProperties)),
    std::make_pair("materialblock-custom", 0u),

    // all other parameter types are invalid for use in a memory block
};

//////////////////////////////////////////////////////////////////////////

using ParseNodeFunction = std::function<void(Material*, pugi::xml_node, char const*, char const*)>;

//////////////////////////////////////////////////////////////////////////

void ParseMaterialXmlNode_Empty(Material* material, pugi::xml_node root, char const* id, char const* type)
{

}

//////////////////////////////////////////////////////////////////////////

void ParseMaterialXmlNode_Texture2D(Material* material, pugi::xml_node root, char const* id, char const* type)
{
    // retrieve from texture library
    char const* texture_value = root.attribute("value").as_string("engine:\\textures\\error");
    u32 const binding = root.attribute("binding").as_uint();

    Ref<MaterialParameterTexture2D> newTextureParameter = material->FindOrAddParameter<MaterialParameterTexture2D>(id);
    newTextureParameter->SetBlockBinding(binding);
    newTextureParameter->SetData(TextureLibrary::GetTexture(texture_value));
}

//////////////////////////////////////////////////////////////////////////

void ParseMaterialXmlNode_MaterialBlock(Material* material, pugi::xml_node root, char const* id, char const* type)
{
    // retrieve paramater details
    u32 const binding = root.attribute("binding").as_uint();

    size_t memorySize = root.attribute("memory_size").as_uint(0u);
    auto memorySizeMapItr = MaterialTypeMemorySize.find(root.attribute("type").as_string());
    if (memorySize == 0 && memorySizeMapItr != MaterialTypeMemorySize.end())
    {
        memorySize = memorySizeMapItr->second;
    }
    else if (memorySize == 0)
    {
        printf("Could not located size for material type '%s'!\n", root.attribute("type").as_string());
        return;
    }

    Ref<MaterialParameterBlock> newMaterialBlock = material->FindOrAddParameter<MaterialParameterBlock>(id);
    newMaterialBlock->SetBlockBinding(binding);
    newMaterialBlock->SetDataGeneric({ memorySize, nullptr });
}

//////////////////////////////////////////////////////////////////////////

static ParseNodeFunction ParseNodeFunctions[MaterialParameterType::Max] =
{
    ParseMaterialXmlNode_Empty,         // Float1
    ParseMaterialXmlNode_Empty,         // Float2
    ParseMaterialXmlNode_Empty,         // Float3
    ParseMaterialXmlNode_Empty,         // Float4
    ParseMaterialXmlNode_Texture2D,     // Texture2D
    ParseMaterialXmlNode_MaterialBlock,   // PerFrame
    ParseMaterialXmlNode_MaterialBlock,   // PerObject
    ParseMaterialXmlNode_MaterialBlock,   // Time
    ParseMaterialXmlNode_MaterialBlock,   // Custom
};

//////////////////////////////////////////////////////////////////////////

Material::Material(const Ref<Shader>& associatedShader)
    : shader(associatedShader)
    , renderMode(RenderMode::TriangleList)
{
}

//////////////////////////////////////////////////////////////////////////

Material::~Material()
{
}

//////////////////////////////////////////////////////////////////////////

Ref<Material> Material::Clone()
{
    Ref<Material> cloned = std::make_shared<Material>(shader);
    cloned->renderMode = renderMode;
    for (auto parameter : parameters)
    {
        cloned->parameters.emplace(parameter.first, parameter.second->Clone());
    }

    return cloned;
}

//////////////////////////////////////////////////////////////////////////

void Material::OnDestroy()
{
    if (shader)
    {
        //shader->OnDestroy();
        shader = nullptr;
    }
}

//////////////////////////////////////////////////////////////////////////

void Material::BindProperties()
{
    shader->Bind();

    for (auto& kv : parameters)
    {
        kv.second->SetParameter(shader);
    }
}

//////////////////////////////////////////////////////////////////////////

void Material::Reload()
{
    if (shader != nullptr)
    {
        shader = nullptr;
    }

    const std::string convertedFilePath = FileVolumeManager::GetRealPathFromVirtualPath(path).fullpath;

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(convertedFilePath.c_str());
    if (!result)
        return;

    pugi::xml_node root = doc.root().child("material");
    pugi::xml_node parameters_node = root.child("parameters");

    shader = GetApiManager()->CreateShader(path);

    // parse parameters
    for (pugi::xml_node parameter : parameters_node)
    {
        const char* parameter_id = parameter.attribute("id").as_string();
        const char* parameter_type = parameter.attribute("type").as_string();

        MaterialParameterType::Enum paramType = MaterialTypeMap.at(parameter_type);
        ParseNodeFunctions[paramType](this, parameter, parameter_id, parameter_type);
    }

}

//////////////////////////////////////////////////////////////////////////

Ref<Material> Material::LoadFromFile(const std::string& filepath)
{
    const std::string convertedFilePath = FileVolumeManager::GetRealPathFromVirtualPath(filepath).fullpath;

    pugi::xml_document doc;
    
    pugi::xml_parse_result result = doc.load_file(convertedFilePath.c_str());
    if (!result)
        return nullptr;

    pugi::xml_node root = doc.root().child("material");

    //Ref<Shader> shader;
    Ref<Material> material;

    if (pugi::xml_attribute based_on = root.attribute("based_on"))
    {
        // we should clone this material from the based_on material.
        // this requires the based_on material to be loaded first.

        material = MaterialLibrary::GetMaterial(based_on.as_string())->Clone();
    }
    else
    {
        pugi::xml_node program_node = root.child("program");

        Ref<Shader> shader = GetApiManager()->CreateShader(program_node);
        material = std::make_shared<Material>(shader);
    }

    RenderMode::Enum renderMode = material->renderMode;
    if (pugi::xml_attribute renderModeAttribute = root.attribute("render_mode"))
    {
        renderMode = RenderMode::FromString(renderModeAttribute.as_string());
    }

    // extract material nodes
    pugi::xml_node parameters_node = root.child("parameters");

    // parse parameters
    for (pugi::xml_node parameter : parameters_node)
    {
        const char* parameter_id = parameter.attribute("id").as_string();
        const char* parameter_type = parameter.attribute("type").as_string();

        MaterialParameterType::Enum paramType = MaterialTypeMap.at(parameter_type);
        ParseNodeFunctions[paramType](material.get(), parameter, parameter_id, parameter_type);
    }

    // keep a reference to the VIRTUAL filepath
    material->path = filepath;
    material->renderMode = renderMode;

    return material;
}

//////////////////////////////////////////////////////////////////////////
