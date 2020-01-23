//////////////////////////////////////////////////////////////////////////
//    File        	: TextureLibrary.h
//    Created By    : Jack Spink
//    Created On 	: [17/10/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Singleton.h"

//////////////////////////////////////////////////////////////////////////

#include <string>
#include <unordered_map>

//////////////////////////////////////////////////////////////////////////

class Texture2D;

//////////////////////////////////////////////////////////////////////////

class TextureLibrary : public Singleton<TextureLibrary>
{
public:
    static void Initialise();

    static void RegisterTexture(const std::string& id, const std::string& path);
    static void RegisterTexture(const std::string& id, Ref<Texture2D> texture);

    static Ref<Texture2D> GetTexture(const std::string& id);

private:
    void InitialiseImpl();

    void RegisterTextureImpl(const std::string& id, const std::string& path);
    void RegisterTextureImpl(const std::string& id, Ref<Texture2D> texture);

    Ref<Texture2D> GetTextureImpl(const std::string& id);

private:
    std::unordered_map<std::string, Ref<Texture2D>> loadedTextures;
};

//////////////////////////////////////////////////////////////////////////

static inline Ref<TextureLibrary> GetTextureLibrary() { return TextureLibrary::Get(); }

//////////////////////////////////////////////////////////////////////////
