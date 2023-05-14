#include "stdafx.h"
#include "FmodMgr.h"
#include "ResourceMgr.h"
#include "TimeMgr.h"

using namespace reality;

bool FmodMgr::Init()
{
    // FMOD 시스템 생성
    CreateFmodSystem();
    // FMOD 채널 그룹 생성
    CreateFmodChannelGroup();
    // Create Sound Pool
    if(sound_pool.empty())
        CreateSoundPool();

    music_volume_ = 0.5f;
    sfx_volume_ = 0.5f;

    return true;
}

int FmodMgr::Update()
{
    fmod_system_->update();

    CheckPlayingPool();

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
    FMOD_VECTOR listenerPos = { 0.0f, 0.0f, 0.0f };
    FMOD_VECTOR listenerVel = { 0.0f, 0.0f, 0.0f };
    FMOD_VECTOR up = { 0.0f, 1.0f, 0.0f };
    FMOD_VECTOR forward = { 1.0f, 0.0f, 0.0f };
    hr = fmod_system_->set3DListenerAttributes(0, &listenerPos, &listenerVel, &forward, &up); 
    hr = fmod_system_->set3DSettings(1.0f, 1.0f, 1.0f);
}

void FmodMgr::CheckPlayingPool()
{
    for (auto iter = sound_play_list.begin(); iter != sound_play_list.end(); )
    {
        Sound* sound = *iter;
        // 재생 중인 사운드의 현재 시간 갱신
        sound->channel->getPosition(&sound->current_time, FMOD_TIMEUNIT_MS);

        // 갱신된 사운드가 끝났다면 초기화 하고 Sound만 풀에 넣기
        if (!sound->looping && sound->current_time >= sound->total_time)
        {
            ReturnToSoundPool(sound);

            iter = sound_play_list.erase(iter);

            if (iter == sound_play_list.end())
                break;
        }
        iter++;
    }
}

void FmodMgr::SetPlayingSoundVolume()
{
    for (auto sound : sound_play_list)
    {
        float volume;
        sound->channel->getVolume(&volume);
        switch (sound->type)
        {
        case MUSIC:
            sound->channel->setVolume(sound->constant_volume * music_volume_);
            break;
        case SFX:
            sound->channel->setVolume(sound->constant_volume * sfx_volume_);
            break;
        }

    }
}

void FmodMgr::Play(string sound_name, SoundType sound_type, bool looping, float volume, FXMVECTOR generate_pos)
{
    float distance = XMVectorGetX(XMVector3Length(generate_pos));

    //float attenuated_vol = (max_distance_ - distance) / max_distance_ <= 0 ? 0.0f : (max_distance_ - distance) / max_distance_;

    float speed = 10.0f;
    XMVECTOR nor_vel = XMVector3Normalize(generate_pos) * speed;// XMVector3Normalize(-generate_pos);

    FMOD_VECTOR pos = { generate_pos.m128_f32[0], generate_pos.m128_f32[1], generate_pos.m128_f32[2] };

    FMOD_VECTOR vel;
    vel.x = XMVectorGetX(nor_vel);
    vel.y = XMVectorGetY(nor_vel);
    vel.z = XMVectorGetZ(nor_vel);

    FMOD_RESULT fr;

    Sound* sound_data = LoadSoundFromPool();
    sound_data->sound_filename = sound_name;
    sound_data->type = sound_type;
    sound_data->sound = RESOURCE->UseResource<FMOD::Sound>(sound_name);
    sound_data->sound->getLength(&sound_data->total_time, FMOD_TIMEUNIT_MS);
    sound_data->constant_volume = volume;// *attenuated_vol;
    //fr = sound_data->sound->set3DMinMaxDistance(1.0f, 3000.0f);

    sound_data->looping = looping;
    fr = sound_data->sound->setMode(looping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);

    if (sound_type == MUSIC)
    {
        //fr = FMOD_MGR->fmod_system()->playSound(sound_data->sound, FMOD_MGR->music_channel_group(), false, &sound_data->channel);
        fr = FMOD_MGR->fmod_system()->playSound(sound_data->sound, nullptr, false, &sound_data->channel);
        fr = sound_data->channel->setVolume(sound_data->constant_volume * FMOD_MGR->music_volume_);
    }
    else
    {
        //fr = FMOD_MGR->fmod_system()->playSound(sound_data->sound, FMOD_MGR->sfx_channel_group(), false, &sound_data->channel);
        fr = FMOD_MGR->fmod_system()->playSound(sound_data->sound, nullptr, false, &sound_data->channel);
        fr = sound_data->channel->setVolume(sound_data->constant_volume * FMOD_MGR->sfx_volume_);
    }
    fr = sound_data->channel->set3DMinMaxDistance(1000.0f, max_distance_);
    fr = sound_data->channel->set3DLevel(1.0f);
    fr = sound_data->channel->set3DAttributes(&pos, &vel);

    sound_play_list.push_back(sound_data);
}

void FmodMgr::PlayBackground(string sound_name, bool looping, float fade_in, float volume)
{
    static float timer = 0.0f;

    timer += TM_DELTATIME;
    float time_lerp = min(1.0f, timer / fade_in);

    for (auto sound : sound_play_list)
    {
        if (sound->sound_filename == sound_name)
        {
            sound->channel->setVolume(sound->constant_volume * FMOD_MGR->music_volume_ * time_lerp);
            return;
        }
    }

    Sound* sound_data = LoadSoundFromPool();
    sound_data->sound_filename = sound_name;
    sound_data->type = MUSIC;
    sound_data->sound = RESOURCE->UseResource<FMOD::Sound>(sound_name);
    sound_data->sound->getLength(&sound_data->total_time, FMOD_TIMEUNIT_MS);
    sound_data->constant_volume = volume;
    sound_data->looping = looping;
    sound_data->sound->setMode(looping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);

    FMOD_RESULT hr;
    hr = FMOD_MGR->fmod_system()->playSound(sound_data->sound, FMOD_MGR->music_channel_group(), false, &sound_data->channel);

    sound_play_list.push_back(sound_data);
}

void FmodMgr::Stop(string sound_name)
{
    for (const auto& sound : sound_play_list)
    {
        if (sound->sound_filename == sound_name)
        {
            sound->total_time = 0;
            sound->looping = false;
        }
        
    }
    
}

bool FmodMgr::FadeOutDelete(string sound_name, float fade_out)
{
    static float timer = 0.0f;
    timer += TM_DELTATIME;
    float time_lerp = max(0.0f, 1.0f - timer / fade_out);

    if (time_lerp <= 0.0001f)
    {
        Stop(sound_name);
        return true;
    }

    for (const auto& sound : sound_play_list)
    {
        if (sound->sound_filename == sound_name)
        {
            if(sound->type == MUSIC)
                sound->channel->setVolume(sound->constant_volume * FMOD_MGR->GetMusicVolume() * time_lerp);
            else if(sound->type == SFX)
                sound->channel->setVolume(sound->constant_volume * FMOD_MGR->GetSFXVolume() * time_lerp);
        }
            
    }
    return false;
}

void FmodMgr::CreateSoundPool()
{
    for (int i = 0; i < POOL_SIZE; i++)
    {
        Sound* newSound = new Sound;
        sound_pool.push(newSound);
    }

}

void FmodMgr::ReturnToSoundPool(Sound* sound)
{
    sound->channel->stop();
    sound->channel = nullptr;
    sound->sound_filename = "";
    sound->total_time = 0;
    sound->current_time = 0;
    sound->constant_volume = 0;

    sound_pool.push(sound);
}

Sound* FmodMgr::LoadSoundFromPool()
{
    if (sound_pool.size() == 0)
        CreateSoundPool();

    Sound* sound = sound_pool.front();
    sound_pool.pop();
    return sound;
}

void FmodMgr::CreateFmodChannelGroup()
{
    FMOD_RESULT fr;
    fr = fmod_system_->createChannelGroup("sfxChannelGroup", &sfx_channel_group_);
    fr = fmod_system_->createChannelGroup("musicChannelGroup", &music_channel_group_);

    //fr = sfx_channel_group_->set3DMinMaxDistance(min_distance_, max_distance_);
    //fr = music_channel_group_->set3DLevel(1.0f);
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

    SetPlayingSoundVolume();
}

void FmodMgr::SetSFXVolume(float volume)
{
    sfx_volume_ = volume;

    SetPlayingSoundVolume();
}
