#include "SoundSystem.h"
#include <io.h>

using namespace KGCA41B;


void SoundSystem::OnCreate(entt::registry& reg)
{
    sound_directory_address_ = SOUND_PATH;
    if (sound_directory_address_.empty())
        return;

    // FMOD 시스템 생성
    CreateFmodSystem();
    
    // FMOD 채널 그룹 생성
    CreateFmodChannelGroup();

    LoadDir(sound_directory_address_);
}

void SoundSystem::OnUpdate(entt::registry& reg)
{
    CheckGenerators(reg);

    CheckPlayingPool();

    fmod_system_->update();
}

void SoundSystem::OnRelease()
{
    for (auto& pair : sound_resource_list_)
    {
        pair.second->release();
    }

    sfx_channel_group_->release();
    music_channel_group_->release();

    fmod_system_->release();
}

void SoundSystem::CheckGenerators(entt::registry& reg)
{
    // 업데이트에서는 사운드 제너레이터 컴포넌트를 가지고 있는 엔티티들이 사운드 큐를 가지고 있으면 사운드를 재생한다.
    auto generators = reg.view<SoundGenerator>();
    for (auto entity : generators)
    {
        auto& generator = generators.get<SoundGenerator>(entity);
        while (generator.sound_queue_list.size() != 0)
        {
            auto queue = generator.sound_queue_list.front();

            // 리스너 컴포넌트를 가지고 있는 엔티티들에 대해 3D 벡터 계산 후 사운드 설정
            auto listeners = reg.view<SoundListener>();
            for (auto entity2 : listeners)
            {
                auto& generator_transform = reg.get<Transform>(entity);
                auto& listener_transform = reg.get<Transform>(entity2);
                // TODO : Transform 수정 후 고칠 예정
                XMVECTOR genertor_position = XMVectorSet(generator_transform.world_matrix.r[3].m128_f32[0], generator_transform.world_matrix.r[3].m128_f32[1],
                    generator_transform.world_matrix.r[3].m128_f32[2], 0);
                XMVECTOR listener_position = XMVectorSet(listener_transform.world_matrix.r[3].m128_f32[0], listener_transform.world_matrix.r[3].m128_f32[1],
                    listener_transform.world_matrix.r[3].m128_f32[2], 0);
                XMVECTOR pos = genertor_position - listener_position;
                Play(queue.sound_filename, queue.sound_type, queue.is_looping, queue.sound_volume, pos);
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
        if (sound->current_time >= sound->total_time)
        {
            sound->channel->stop();
            sound->channel = nullptr;
            sound->sound_filename = L"";
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

void SoundSystem::Play(wstring sound_name, SoundType sound_type, bool looping, float volume, FXMVECTOR generate_pos)
{
    if (sound_resource_list_.find(sound_name) == sound_resource_list_.end())
        return;

    // TODO : 3DAttrubutes 인자 Velocity 값 조정 필요, 3DLevel 값 조정 필요
    FMOD_VECTOR pos = { generate_pos.m128_f32[0], generate_pos.m128_f32[1], generate_pos.m128_f32[2] };

    FMOD_VECTOR vel;
    vel.x = -pos.x;
    vel.y = -pos.y;
    vel.z = -pos.z;

    // TODO : 사운드 풀을 이용해 사운드 데이터 가져오기
    Sound* sound_data = LoadSoundFromPool();
    sound_data->sound_filename = sound_name;
    sound_data->sound = sound_resource_list_[sound_name];

    //sound_data->sound->set3DMinMaxDistance(0, 10);
    sound_data->sound->setMode(looping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);

    if(sound_type == MUSIC)
        fmod_system_->playSound(sound_data->sound, music_channel_group_, false, &sound_data->channel);
    else    
        fmod_system_->playSound(sound_data->sound, sfx_channel_group_, false, &sound_data->channel);
        
    sound_data->channel->setVolume(volume);
    sound_data->channel->set3DAttributes(&pos, &vel);
    sound_data->channel->set3DLevel(6);

    sound_play_list.push_back(sound_data);
}

void KGCA41B::SoundSystem::CreateFmodSystem()
{
    FMOD_RESULT hr;

    // FMOD 시스템 생성
    hr = FMOD::System_Create(&fmod_system_);
    hr = fmod_system_->init(CHANNEL_MAX_COUNT, FMOD_INIT_NORMAL, nullptr);
}

void KGCA41B::SoundSystem::CreateFmodChannelGroup()
{
    FMOD_RESULT hr;
    hr = fmod_system_->createChannelGroup("sfxChannelGroup", &sfx_channel_group_);
    hr = fmod_system_->createChannelGroup("musicChannelGroup", &music_channel_group_);
}

void SoundSystem::LoadDir(wstring directory_address)
{
    std::wstring wholeAddr = directory_address + L"*.*";
    intptr_t handle;
    struct _wfinddata_t fd;
    handle = _wfindfirst(wholeAddr.c_str(), &fd);

    // 못찾으면 리턴
    if (handle == -1L) return;

    do {
        if ((fd.attrib & _A_SUBDIR) && (fd.name[0] != '.'))
        {
            LoadDir(directory_address + fd.name + L"/");
        }
        else if (fd.name[0] != '.')
        {
            LoadFile(directory_address + fd.name);
            
        }
    } while (_wfindnext(handle, &fd) == 0);
}

void SoundSystem::LoadFile(wstring file_address)
{
    FMOD::Sound* newSound;

    FMOD_RESULT hr = fmod_system_->createSound(to_wm(file_address).c_str(), (FMOD_MODE)(FMOD_3D), nullptr, &newSound);
    if (hr != FMOD_OK)
    {
        return;
    }
    sound_resource_list_.insert({ file_address, newSound});
}

void KGCA41B::SoundSystem::CreateSoundPool()
{
    Sound* init_sound_data = new Sound;
    for (int i = 0; i < POOL_SIZE; i++)
    {
        Sound* newSound = new Sound(*init_sound_data);
        sound_pool.push(newSound);
    }
        
    delete init_sound_data;
}

Sound* KGCA41B::SoundSystem::LoadSoundFromPool()
{
    if (sound_pool.size() == 0)
        CreateSoundPool();

    Sound* sound = sound_pool.front();
    sound_pool.pop();
    return sound;
}
