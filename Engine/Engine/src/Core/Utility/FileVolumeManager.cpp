//////////////////////////////////////////////////////////////////////////
//    File        	: FileVolumeManager.cpp
//    Created By    : Jack Spink
//    Created On 	: [8/1/2020]
//////////////////////////////////////////////////////////////////////////

#include "Core/Utility/FileVolumeManager.h"
#include "Core/Utility/FileVolume.h"

//////////////////////////////////////////////////////////////////////////

#include <filesystem>

//////////////////////////////////////////////////////////////////////////

#if RELEASE

std::string GameRootPath = std::filesystem::current_path().string() + "\\";
std::string AssetRootPath = GameRootPath + "assets\\";

#else

static const std::string GameRootPath = std::filesystem::current_path().string() + "\\";
static const std::string AssetRootPath = std::string(DEBUG_ASSETS_ROOT) + "\\";

#endif

//////////////////////////////////////////////////////////////////////////

std::string ExtractKeyFromVirtualPath(const std::string& virtualpath)
{
    size_t length = virtualpath.find_first_of("\\");
    if (length != std::string::npos)
    {
        return virtualpath.substr(0, length + 1);
    }
    std::filesystem::current_path().string();
    return "";
}

//////////////////////////////////////////////////////////////////////////

FileVolumeManager::FileVolumeManager()
{
    // setup the game volume
    if (Ref<FileVolume> gameVolume = std::make_shared<FileVolume>("game:\\", GameRootPath))
    {
        fileVolumes.emplace(gameVolume->GetVirtualRoot(), gameVolume);
    }
    
    // setup the asset volume
    if (Ref<FileVolume> assetVolume = std::make_shared<FileVolume>("assets:\\", AssetRootPath))
    {
        fileVolumes.emplace(assetVolume->GetVirtualRoot(), assetVolume);
    }
}

//////////////////////////////////////////////////////////////////////////

PathResult FileVolumeManager::GetRealPathFromVirtualPath(const std::string& virtualpath)
{
    if (Ref<FileVolumeManager> manager = GetFileVolumeManager())
    {
        return manager->GetRealPathFromVirtualPathImpl(virtualpath);
    }

    return {};
}

//////////////////////////////////////////////////////////////////////////

Ref<FileVolume> FileVolumeManager::GetFileVolumeFromVirtualPath(const std::string& virtualpath)
{
    if (Ref<FileVolumeManager> manager = GetFileVolumeManager())
    {
        return manager->GetFileVolumeFromVirtualPathImpl(virtualpath);
    }

    return nullptr;
}

//////////////////////////////////////////////////////////////////////////

PathResult FileVolumeManager::GetRealPathFromVirtualPathImpl(const std::string& virtualpath)
{
    std::string key = ExtractKeyFromVirtualPath(virtualpath);
    auto it = fileVolumes.find(key);
    if (it != fileVolumes.end())
    {
        return it->second->GetRealPathFromVirtualPath(virtualpath);
    }

    return {};
}

//////////////////////////////////////////////////////////////////////////

Ref<FileVolume> FileVolumeManager::GetFileVolumeFromVirtualPathImpl(const std::string& virtualpath)
{
    std::string key = ExtractKeyFromVirtualPath(virtualpath);
    auto it = fileVolumes.find(key);
    if (it != fileVolumes.end())
    {
        return it->second;
    }

    return nullptr;
}

//////////////////////////////////////////////////////////////////////////
