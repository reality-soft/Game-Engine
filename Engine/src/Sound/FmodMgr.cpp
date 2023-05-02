#include "stdafx.h"
#include "FmodMgr.h"
#include "ResourceMgr.h"
#include "TimeMgr.h"

using namespace reality;

bool FmodMgr::Init()
{
    // FMOD �ý��� ����
    CreateFmodSystem();
    // FMOD ä�� �׷� ����
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
}

void FmodMgr::CheckPlayingPool()
{
    for (auto iter = sound_play_list.begin(); iter != sound_play_list.end(); )
    {
        Sound* sound = *iter;
        // ��� ���� ������ ���� �ð� ����
        sound->channel->getPosition(&sound->current_time, FMOD_TIMEUNIT_MS);

        // ���ŵ� ���尡 �����ٸ� �ʱ�ȭ �ϰ� Sound�� Ǯ�� �ֱ�
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
            sound->channel->setVolume(volume * music_volume_);
            break;
        case SFX:
            sound->channel->setVolume(volume * sfx_volume_);
            break;
        }

    }
}

void FmodMgr::Play(string sound_name, SoundType sound_type, bool looping, float volume, FXMVECTOR generate_pos)
{
    FMOD_VECTOR pos = { generate_pos.m128_f32[0], generate_pos.m128_f32[1], generate_pos.m128_f32[2] };

    FMOD_VECTOR vel;
    vel.x = -pos.x;
    vel.y = -pos.y;
    vel.z = -pos.z;

    Sound* sound_data = LoadSoundFromPool();
    sound_data->sound_filename = sound_name;
    sound_data->type = sound_type;
    sound_data->sound = RESOURCE->UseResource<FMOD::Sound>(sound_name);
    sound_data->sound->getLength(&sound_data->total_time, FMOD_TIMEUNIT_MS);
    sound_data->constant_volume = volume;

    //sound_data->sound->set3DMinMaxDistance(0, 10);
    sound_data->looping = looping;
    sound_data->sound->setMode(looping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);

    FMOD_RESULT hr;

    if (sound_type == MUSIC)
    {
        hr = FMOD_MGR->fmod_system()->playSound(sound_data->sound, FMOD_MGR->music_channel_group(), false, &sound_data->channel);
        sound_data->channel->setVolume(volume * FMOD_MGR->music_volume_);
    }
    else
    {
        hr = FMOD_MGR->fmod_system()->playSound(sound_data->sound, FMOD_MGR->sfx_channel_group(), false, &sound_data->channel);
        sound_data->channel->setVolume(volume * FMOD_MGR->sfx_volume_);
    }
    sound_data->channel->set3DAttributes(&pos, &vel);
    sound_data->channel->set3DLevel(6);

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

bool FmodMgr::FadeOutDelete(string sound_name, float fade_out)
{

    static float timer = 0.0f;
    timer += TM_DELTATIME;
    float time_lerp = max(0.0f, 1.0f - timer / fade_out);

    if (time_lerp <= 0.0001f)
    {
        for (const auto& sound : sound_play_list)
        {
            if(sound->sound_filename == sound_name)
                ReturnToSoundPool(sound);
        }
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

    SetPlayingSoundVolume();
}

void FmodMgr::SetSFXVolume(float volume)
{
    sfx_volume_ = volume;

    SetPlayingSoundVolume();
}
