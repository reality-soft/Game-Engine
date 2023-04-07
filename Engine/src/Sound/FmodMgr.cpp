#include "stdafx.h"
#include "FmodMgr.h"

using namespace reality;

bool FmodMgr::Init()
{
    // FMOD 시스템 생성
    CreateFmodSystem();
    // FMOD 채널 그룹 생성
    CreateFmodChannelGroup();

    music_volume_ = 5.0f;
    sfx_volume_ = 5.0f;

    return true;
}

int FmodMgr::Update()
{
    fmod_system_->update();
    return 0;
}

void FmodMgr::Release()
{
    sfx_channel_group_->release();
    music_channel_group_->release();

    fmod_system_->release();
}

void FmodMgr::CreateFmodSystem()
{
    FMOD_RESULT hr;
    hr = FMOD::System_Create(&fmod_system_);
    hr = fmod_system_->init(100, FMOD_INIT_NORMAL, nullptr);
}

void FmodMgr::CreateFmodChannelGroup()
{
    FMOD_RESULT hr;
    hr = fmod_system_->createChannelGroup("sfxChannelGroup", &sfx_channel_group_);
    hr = fmod_system_->createChannelGroup("musicChannelGroup", &music_channel_group_);
}

float FmodMgr::GetMusicVolume()
{
    return music_volume_;
}

float FmodMgr::GetSFXVolume()
{
    return sfx_volume_;
}

void FmodMgr::SetMusicVolume(float volume)
{
    music_volume_ = volume;
}

void FmodMgr::SetSFXVolume(float volume)
{
    sfx_volume_ = volume;
}
