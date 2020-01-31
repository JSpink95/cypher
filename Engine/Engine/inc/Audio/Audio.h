//////////////////////////////////////////////////////////////////////////
//    File        	: Audio.h
//    Created By    : Jack Spink
//    Created On 	: [31/01/2020]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Singleton.h"

//////////////////////////////////////////////////////////////////////////

#include "irrklang_audio.h"

//////////////////////////////////////////////////////////////////////////

class Audio: public Singleton<Audio>
{
public:
    static inline void PlayAudio(const std::string& path, const bool loop = false)
    {
        if (Ref<Audio> audio = Get())
        {
            audio->PlayAudioImpl(path, loop);
        }
    }

public:
    virtual void OnCreate() override;

private:
    void PlayAudioImpl(const std::string& path, const bool loop);

private:
    ik::ISoundEngine* engine = nullptr;
};

//////////////////////////////////////////////////////////////////////////