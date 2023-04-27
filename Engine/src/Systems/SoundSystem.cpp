#include "stdafx.h"
#include "TimeMgr.h"
#include "SoundSystem.h"
#include "ResourceMgr.h"
#include "FmodMgr.h"
#include <io.h>

using namespace reality;

void SoundSystem::OnUpdate(entt::registry& reg)
{
    CheckGenerators(reg);

    CheckPlayingPool();

    SetPlayingSoundVolume();
}

void reality::SoundSystem::PlayBackground(string sound_name, bool looping, float fade_in, float volume)
{
    static float timer = 0.0f;

    timer += TM_DELTATIME;
    float time_lerp = min(1.0f, timer / fade_in);

    for (auto sound : sound_play_list)
    {
        if (sound->sound_filename == sound_name)
        {
            sound->channel->setVolume(sound->constant_volume * time_lerp);
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

bool reality::SoundSystem::FadeOutDelete(float fade_out)
{
    
    static float timer = 0.0f;
    timer += TM_DELTATIME;
    float time_lerp = max(0.0f, 1.0f - timer / fade_out);

    if (time_lerp <= 0.0001f)
    {
        for (auto sound : sound_play_list)
        {
            sound->sound->release();
            delete sound;
            sound = nullptr;
        }
        sound_play_list.clear();
        return true;
    }

    for (const auto& sound : sound_play_list)
    {
        sound->channel->setVolume(sound->constant_volume * time_lerp);
    }

    return false;
}

void SoundSystem::CheckGenerators(entt::registry& reg)
{
    // 업데이트에서는 사운드 제너레이터 컴포넌트를 가지고 있는 엔티티들이 사운드 큐를 가지고 있으면 사운드를 재생한다.
    auto generators = reg.view<C_SoundGenerator>();
    for (auto entity : generators)
    {
        auto& generator = generators.get<C_SoundGenerator>(entity);
        while (generator.sound_queue_list.size() != 0)
        {
            auto queue = generator.sound_queue_list.front();

            // 리스너 컴포넌트를 가지고 있는 엔티티들에 대해 3D 벡터 계산 후 사운드 설정
            auto listeners = reg.view<C_SoundListener>();
            for (auto entity2 : listeners)
            {
                auto& listener_transform = reg.get<C_CapsuleCollision>(entity2);
                auto generator_transform = reg.try_get<C_Transform>(entity);
                if (generator_transform)
                {
                    XMVECTOR genertor_position = XMVectorSet(generator_transform->world.r[3].m128_f32[0], generator_transform->world.r[3].m128_f32[1],
                        generator_transform->world.r[3].m128_f32[2], 0);
                    XMVECTOR listener_position = XMVectorSet(listener_transform.world.r[3].m128_f32[0], listener_transform.world.r[3].m128_f32[1],
                        listener_transform.world.r[3].m128_f32[2], 0);
                    XMVECTOR pos = genertor_position - listener_position;
                    Play(queue.sound_filename, queue.sound_type, queue.is_looping, queue.sound_volume, pos);
                }

                auto generator_transform2 = reg.try_get<C_CapsuleCollision>(entity2);
                if (generator_transform2)
                {
                    XMVECTOR generator_position2 = XMVectorSet(generator_transform2->world.r[3].m128_f32[0], generator_transform2->world.r[3].m128_f32[1],
                        generator_transform2->world.r[3].m128_f32[2], 0);
                    XMVECTOR listener_position2 = XMVectorSet(listener_transform.world.r[3].m128_f32[0], listener_transform.world.r[3].m128_f32[1],
                        listener_transform.world.r[3].m128_f32[2], 0);
                    XMVECTOR pos2 = generator_position2 - listener_position2;
                    Play(queue.sound_filename, queue.sound_type, queue.is_looping, queue.sound_volume, pos2);
                }
                
            }

            // 재생했다면 큐에서 제거
            generator.sound_queue_list.pop();
        }
    }
}

void SoundSystem::CheckPlayingPool()
{
    for (auto iter = sound_play_list.begin(); iter != sound_play_list.end(); )
    {
        Sound* sound = *iter;
        // 재생 중인 사운드의 현재 시간 갱신
        sound->channel->getPosition(&sound->current_time, FMOD_TIMEUNIT_MS);

        // 갱신된 사운드가 끝났다면 초기화 하고 Sound만 풀에 넣기
        if (!sound->looping && sound->current_time >= sound->total_time)
        {
            sound->channel->stop();
            sound->channel = nullptr;
            sound->sound_filename = "";
            sound->total_time = 0;
            sound->current_time = 0;

            iter = sound_play_list.erase(iter);

            sound_pool.push(sound);

            if (iter == sound_play_list.end())
                break;
        }
        iter++;
    }
}

void reality::SoundSystem::SetPlayingSoundVolume()
{
    for (auto sound : sound_play_list)
    {
        float volume;
        sound->channel->getVolume(&volume);
        switch (sound->type)
        {
        case MUSIC:
            sound->channel->setVolume(volume * FMOD_MGR->GetMusicVolume());
            break;
        case SFX:
            sound->channel->setVolume(volume * FMOD_MGR->GetSFXVolume());
            break;
        }
        
    }
}

void SoundSystem::Play(string sound_name, SoundType sound_type, bool looping, float volume, FXMVECTOR generate_pos)
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

    //sound_data->sound->set3DMinMaxDistance(0, 10);
    sound_data->looping = looping;
    sound_data->sound->setMode(looping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);

    FMOD_RESULT hr;

    if (sound_type == MUSIC)
    {
        hr = FMOD_MGR->fmod_system()->playSound(sound_data->sound, FMOD_MGR->music_channel_group(), false, &sound_data->channel);
        sound_data->channel->setVolume(volume);
    }
    else
    {
        hr = FMOD_MGR->fmod_system()->playSound(sound_data->sound, FMOD_MGR->sfx_channel_group(), false, &sound_data->channel);
        sound_data->channel->setVolume(volume);
    }
    sound_data->channel->set3DAttributes(&pos, &vel);
    sound_data->channel->set3DLevel(6);

    sound_play_list.push_back(sound_data);
}

void reality::SoundSystem::CreateSoundPool()
{
    Sound* init_sound_data = new Sound;
    for (int i = 0; i < POOL_SIZE; i++)
    {
        Sound* newSound = new Sound(*init_sound_data);
        sound_pool.push(newSound);
    }
        
    delete init_sound_data;
}

Sound* reality::SoundSystem::LoadSoundFromPool()
{
    if (sound_pool.size() == 0)
        CreateSoundPool();

    Sound* sound = sound_pool.front();
    sound_pool.pop();
    return sound;
}
