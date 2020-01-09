//////////////////////////////////////////////////////////////////////////
//    File        	: FileVolumeManager.h
//    Created By    : Jack Spink
//    Created On 	: [8/1/2020]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Singleton.h"
#include "Core/Utility/FileVolume.h"

//////////////////////////////////////////////////////////////////////////

class FileVolumeManager : public Singleton<FileVolumeManager>
{
public:
    FileVolumeManager();

    static PathResult GetRealPathFromVirtualPath(const std::string& virtualpath);
    static Ref<FileVolume> GetFileVolumeFromVirtualPath(const std::string& virtualpath);

private:

    PathResult GetRealPathFromVirtualPathImpl(const std::string& virtualpath);
    Ref<FileVolume> GetFileVolumeFromVirtualPathImpl(const std::string& virtualpath);

private:
    std::unordered_map<std::string, Ref<FileVolume>> fileVolumes;
};

//////////////////////////////////////////////////////////////////////////

static inline Ref<FileVolumeManager> GetFileVolumeManager() { return FileVolumeManager::Get(); }

//////////////////////////////////////////////////////////////////////////
