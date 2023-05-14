#pragma once
#include "Sound.h"

#define CHANNEL_MAX_COUNT 1000
#define POOL_SIZE 10

namespace reality 
{
    class DLL_API FmodMgr
    {
        SINGLETON(FmodMgr)
#define FMOD_MGR FmodMgr::GetInst()
    private:
        FMOD::System* fmod_system_;
        FMOD::ChannelGroup* sfx_channel_group_;
        FMOD::ChannelGroup* music_channel_group_;
        float sfx_volume_;
        float music_volume_;
        float min_distance_ = 0.0f;
        float max_distance_ = 30000.0f;
    private:
        list<Sound*>	sound_play_list;
        queue<Sound*>	sound_pool;
    public:
        FMOD::System* fmod_system() {
            return fmod_system_;
        }
        FMOD::ChannelGroup* sfx_channel_group() {
            return sfx_channel_group_;
        }
        FMOD::ChannelGroup* music_channel_group() {
            return music_channel_group_;
        }
    public:
        bool Init();
        int  Update();
        void Release();
    private:
        void CreateFmodSystem();
        void CheckPlayingPool();
        void SetPlayingSoundVolume();

        void CreateSoundPool();
        void ReturnToSoundPool(Sound* sound);
        Sound* LoadSoundFromPool();
    public:
        void CreateFmodChannelGroup(); 
        void Play(string sound_name, SoundType sound_type, bool looping, float volume, FXMVECTOR generate_pos);
        void PlayBackground(string sound_name, bool looping, float fade_in, float volume);
        void Stop(string sound_name);
        bool FadeOutDelete(string sound_name, float fade_out);
    public:
        float GetMusicVolume();
        float GetSFXVolume();
        void  SetMusicVolume(float volume);
        void  SetSFXVolume(float volume);
    };
}

