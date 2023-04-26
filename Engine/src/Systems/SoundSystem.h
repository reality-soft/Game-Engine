#pragma once
#include "System.h"

#define POOL_SIZE 10

namespace reality {
	class DLL_API SoundSystem : public System
	{
	private:
		// 사운드 시스템
		list<Sound*>	sound_play_list;
		queue<Sound*>	sound_pool;
	public:
		virtual void OnCreate(entt::registry& reg) {}
		virtual void OnUpdate(entt::registry& reg);

		void PlayBackground(string sound_name, bool looping, float fade_in, float volume);
		bool FadeOutDelete(float fade_out);

	private:
		// 사운드 시스템
		void CheckGenerators(entt::registry& reg);
		void CheckPlayingPool();
	private:
		void Play(string sound_name, SoundType sound_type, bool looping, float volume, FXMVECTOR generate_pos);
		void CreateSoundPool();
		Sound* LoadSoundFromPool();
	};
}


