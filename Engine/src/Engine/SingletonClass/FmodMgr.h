#pragma once
#include "DllMacro.h"
#include "stdafx.h"

#define CHANNEL_MAX_COUNT 1000

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
    public:
        void CreateFmodChannelGroup();

    };
}

