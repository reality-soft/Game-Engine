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
}

void SoundSystem::PlayBackground(string sound_name, bool looping, float fade_in, float volume)
{
    FMOD_MGR->PlayBackground(sound_name, looping, fade_in, volume);
}

bool SoundSystem::FadeOutDelete(string sound_name, float fade_out)
{
    return FMOD_MGR->FadeOutDelete(sound_name, fade_out);
}

void reality::SoundSystem::ResetFadeTimer()
{
    fade_in_timer = 0.0f;
    fade_out_timer = 0.0f;
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
                    FMOD_MGR->Play(queue.sound_filename, queue.sound_type, queue.is_looping, queue.sound_volume, pos);
                }

                auto generator_transform2 = reg.try_get<C_CapsuleCollision>(entity2);
                if (generator_transform2)
                {
                    XMVECTOR generator_position2 = XMVectorSet(generator_transform2->world.r[3].m128_f32[0], generator_transform2->world.r[3].m128_f32[1],
                        generator_transform2->world.r[3].m128_f32[2], 0);
                    XMVECTOR listener_position2 = XMVectorSet(listener_transform.world.r[3].m128_f32[0], listener_transform.world.r[3].m128_f32[1],
                        listener_transform.world.r[3].m128_f32[2], 0);
                    XMVECTOR pos2 = generator_position2 - listener_position2;
                    FMOD_MGR->Play(queue.sound_filename, queue.sound_type, queue.is_looping, queue.sound_volume, pos2);
                }
                
            }

            // 재생했다면 큐에서 제거
            generator.sound_queue_list.pop();
        }
    }
}