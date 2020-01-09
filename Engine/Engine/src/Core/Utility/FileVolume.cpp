//////////////////////////////////////////////////////////////////////////
//    File        	: FileVolume.cpp
//    Created By    : Jack Spink
//    Created On 	: [8/1/2020]
//////////////////////////////////////////////////////////////////////////

#include "Core/Utility/FileVolume.h"

//////////////////////////////////////////////////////////////////////////

#include <filesystem>

//////////////////////////////////////////////////////////////////////////

FileVolume::FileVolume(const std::string& vr, const std::string& r)
    : virtualroot(vr)
    , root(r)
{

}

//////////////////////////////////////////////////////////////////////////

PathResult FileVolume::GetRealPathFromVirtualPath(const std::string& virtualpath) const
{
    PathResult result;

    // 
    // example of virtual path:
    //  assets:/mesh/box.obj
    // expanded to real path:
    // ..resources/assets/mesh/box.obj
    // 

    size_t pos = virtualpath.find(virtualroot);
    if (pos != 0)
    {
        // invalid virtual path for this volume
        return result;
    }

    result.fullpath = virtualpath;
    result.fullpath.replace(pos, pos + virtualroot.length(), root.c_str());
    result.valid = std::filesystem::exists(result.fullpath);

    size_t slashpos = virtualpath.find_last_of("\\");
    result.filename = virtualpath.substr((slashpos == std::string::npos ? 0 : (slashpos + 1)));

    return result;
}

//////////////////////////////////////////////////////////////////////////
