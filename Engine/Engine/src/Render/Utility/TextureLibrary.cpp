//////////////////////////////////////////////////////////////////////////
//    File        	: TextureLibrary.cpp
//    Created By    : Jack Spink
//    Created On 	: [17/10/2019]
//////////////////////////////////////////////////////////////////////////

#include "Render/Utility/TextureLibrary.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Utility/FileVolumeManager.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/ApiManager.h"
#include "Render/Platform/Texture2D.h"

//////////////////////////////////////////////////////////////////////////

void TextureLibrary::Initialise()
{
    Ref<TextureLibrary> library = GetTextureLibrary();
    if (library)
    {
        library->InitialiseImpl();
    }
}

//////////////////////////////////////////////////////////////////////////

void TextureLibrary::RegisterTexture(const std::string& id, const std::string& path)
{
    Ref<TextureLibrary> library = GetTextureLibrary();
    if (library)
    {
        library->RegisterTextureImpl(id, path);
    }
}

//////////////////////////////////////////////////////////////////////////

void TextureLibrary::RegisterTexture(const std::string& id, Ref<Texture2D> texture)
{
    Ref<TextureLibrary> library = GetTextureLibrary();
    if (library)
    {
        library->RegisterTextureImpl(id, texture);
    }
}

//////////////////////////////////////////////////////////////////////////

Ref<Texture2D> TextureLibrary::GetTexture(const std::string& id)
{
    Ref<TextureLibrary> library = GetTextureLibrary();
    if (library)
    {
        return library->GetTextureImpl(id);
    }

    return nullptr;
}

//////////////////////////////////////////////////////////////////////////

void TextureLibrary::InitialiseImpl()
{
    // flat white
    static const float3 whitePixels[2 * 2] =
    {
        float3(1.0f), float3(1.0f),
        float3(1.0f), float3(1.0f),
    };

    // magenta:black checkerboard
    static const float3 errorPixels[8 * 8] =
    {
        float3(1.0f, 0.0f, 1.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 1.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 1.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 1.0f), float3(0.0f, 0.0f, 0.0f),
        float3(0.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 1.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 1.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 1.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 1.0f),
        float3(1.0f, 0.0f, 1.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 1.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 1.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 1.0f), float3(0.0f, 0.0f, 0.0f),
        float3(0.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 1.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 1.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 1.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 1.0f),
        float3(1.0f, 0.0f, 1.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 1.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 1.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 1.0f), float3(0.0f, 0.0f, 0.0f),
        float3(0.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 1.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 1.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 1.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 1.0f),
        float3(1.0f, 0.0f, 1.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 1.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 1.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 1.0f), float3(0.0f, 0.0f, 0.0f),
        float3(0.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 1.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 1.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 1.0f), float3(0.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 1.0f),
    };

    // white:lesswhite checkerboard
    static const float3 devPixels[8 * 8] =
    {
        float3(1.0f), float3(0.6f), float3(1.0f), float3(0.6f), float3(1.0f), float3(0.6f), float3(1.0f), float3(0.6f),
        float3(0.6f), float3(1.0f), float3(0.6f), float3(1.0f), float3(0.6f), float3(1.0f), float3(0.6f), float3(1.0f),
        float3(1.0f), float3(0.6f), float3(1.0f), float3(0.6f), float3(1.0f), float3(0.6f), float3(1.0f), float3(0.6f),
        float3(0.6f), float3(1.0f), float3(0.6f), float3(1.0f), float3(0.6f), float3(1.0f), float3(0.6f), float3(1.0f),
        float3(1.0f), float3(0.6f), float3(1.0f), float3(0.6f), float3(1.0f), float3(0.6f), float3(1.0f), float3(0.6f),
        float3(0.6f), float3(1.0f), float3(0.6f), float3(1.0f), float3(0.6f), float3(1.0f), float3(0.6f), float3(1.0f),
        float3(1.0f), float3(0.6f), float3(1.0f), float3(0.6f), float3(1.0f), float3(0.6f), float3(1.0f), float3(0.6f),
        float3(0.6f), float3(1.0f), float3(0.6f), float3(1.0f), float3(0.6f), float3(1.0f), float3(0.6f), float3(1.0f),
    };

    // sphere texture (with alpha)
    std::vector<float4> spherePixels;
    for (uint x = 0; x < 16; ++x)
    {
        for (uint y = 0; y < 16; ++y)
        {
            float2 const pxy = float2(x + 0.5f, y + 0.5f);
            f32 const dist = glm::distance(pxy, float2(8.0f)) / 8.0f;
            f32 const alpha = glm::clamp(1.0f - dist, 0.0f, 1.0f);

            spherePixels.push_back(float4(alpha));
        }
    }

    // load engine textures
    loadedTextures.insert(std::make_pair("engine:\\textures\\white", GetApiManager()->CreateTexture2D(uint2(2), whitePixels)));
    loadedTextures.insert(std::make_pair("engine:\\textures\\error", GetApiManager()->CreateTexture2D(uint2(8), errorPixels)));
    loadedTextures.insert(std::make_pair("engine:\\textures\\dev", GetApiManager()->CreateTexture2D(uint2(8), devPixels)));
    loadedTextures.insert(std::make_pair("engine:\\textures\\particle-sphere", GetApiManager()->CreateTexture2D(uint2(16), &spherePixels.at(0))));

    PathResult textureAssetPath = FileVolumeManager::GetRealPathFromVirtualPath("assets:\\TextureAssets.xml");
    if (textureAssetPath.valid)
    {
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(textureAssetPath.fullpath.c_str());
        if (result)
        {
            pugi::xml_node root = doc.root().child("textures");
            for (pugi::xml_node textureNode : root)
            {
                const char* filepath = textureNode.attribute("filepath").as_string();

                std::string id = filepath;
                id = id.substr(0, id.find_last_of("."));

#ifdef DEBUG
                printf("Loading texture('%s')\n", id, filepath);
#endif

                RegisterTextureImpl(id, filepath);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void TextureLibrary::RegisterTextureImpl(const std::string& id, const std::string& path)
{
    Ref<Texture2D> texture = GetApiManager()->CreateTexture2D(path);
    loadedTextures.insert(std::make_pair(id, texture));
}

//////////////////////////////////////////////////////////////////////////

void TextureLibrary::RegisterTextureImpl(const std::string& id, Ref<Texture2D> texture)
{
    loadedTextures.insert(std::make_pair(id, texture));
}

//////////////////////////////////////////////////////////////////////////

Ref<Texture2D> TextureLibrary::GetTextureImpl(const std::string& id)
{
    auto it = loadedTextures.find(id);
    if (it != loadedTextures.end())
        return it->second;

    return nullptr;
}

//////////////////////////////////////////////////////////////////////////
