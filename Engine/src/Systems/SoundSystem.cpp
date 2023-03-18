#include "stdafx.h"
#include "SoundSystem.h"
#include "ResourceMgr.h"
#include "FmodMgr.h"
#include <io.h>

using namespace reality;

void SoundSystem::OnUpdate(entt::registry& reg)
{
    CheckGenerators(reg);

    CheckPlayingPool();
}

void SoundSystem::CheckGenerators(entt::registry& reg)
{
    // ������Ʈ������ ���� ���ʷ����� ������Ʈ�� ������ �ִ� ��ƼƼ���� ���� ť�� ������ ������ ���带 ����Ѵ�.
    auto generators = reg.view<C_SoundGenerator>();
    for (auto entity : generators)
    {
        auto& generator = generators.get<C_SoundGenerator>(entity);
        while (generator.sound_queue_list.size() != 0)
        {
            auto queue = generator.sound_queue_list.front();

            // ������ ������Ʈ�� ������ �ִ� ��ƼƼ�鿡 ���� 3D ���� ��� �� ���� ����
            auto listeners = reg.view<C_SoundListener>();
            for (auto entity2 : listeners)
            {
                auto& generator_transform = reg.get<C_Transform>(entity);
                auto& listener_transform = reg.get<C_Transform>(entity2);
                // TODO : Transform ���� �� ��ĥ ����
                XMVECTOR genertor_position = XMVectorSet(generator_transform.world.r[3].m128_f32[0], generator_transform.world.r[3].m128_f32[1],
                    generator_transform.world.r[3].m128_f32[2], 0);
                XMVECTOR listener_position = XMVectorSet(listener_transform.world.r[3].m128_f32[0], listener_transform.world.r[3].m128_f32[1],
                    listener_transform.world.r[3].m128_f32[2], 0);
                XMVECTOR pos = genertor_position - listener_position;
                Play(queue.sound_filename, queue.sound_type, queue.is_looping, queue.sound_volume, pos);
            }

            // ����ߴٸ� ť���� ����
            generator.sound_queue_list.pop();
        }
    }
}

void SoundSystem::CheckPlayingPool()
{
    for (auto iter = sound_play_list.begin(); iter != sound_play_list.end(); )
    {
        Sound* sound = *iter;
        // ��� ���� ������ ���� �ð� ����
        sound->channel->getPosition(&sound->current_time, FMOD_TIMEUNIT_MS);

        // ���ŵ� ���尡 �����ٸ� �ʱ�ȭ �ϰ� Sound�� Ǯ�� �ֱ�
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

    if(sound_type == MUSIC)
        hr = FMOD_MGR->fmod_system()->playSound(sound_data->sound, FMOD_MGR->music_channel_group(), false, &sound_data->channel);
    else    
        hr = FMOD_MGR->fmod_system()->playSound(sound_data->sound, FMOD_MGR->sfx_channel_group(), false, &sound_data->channel);
        
    sound_data->channel->setVolume(volume);
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
