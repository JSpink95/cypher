//////////////////////////////////////////////////////////////////////////
//    File        	: Audio.cpp
//    Created By    : Jack Spink
//    Created On 	: [31/01/2020]
//////////////////////////////////////////////////////////////////////////

#include "Audio/Audio.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Utility/FileVolumeManager.h"

//////////////////////////////////////////////////////////////////////////

void Audio::OnCreate()
{
    engine = ik::createIrrKlangDevice();
}

//////////////////////////////////////////////////////////////////////////

void Audio::PlayAudioImpl(const std::string& path, const bool loop)
{
    if (engine != nullptr)
    {
        const std::string truePath = FileVolumeManager::GetRealPathFromVirtualPath(path).fullpath;
        engine->play2D(truePath.c_str(), loop);
    }
}

//////////////////////////////////////////////////////////////////////////
