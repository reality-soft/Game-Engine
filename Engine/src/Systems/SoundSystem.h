#pragma once
#include "System.h"

#define POOL_SIZE 10

namespace reality {
	class DLL_API SoundSystem : public System
	{
	private:
		// ���� �ý���
		list<Sound*>	sound_play_list;
		queue<Sound*>	sound_pool;
	public:
		virtual void OnCreate(entt::registry& reg) {}
		virtual void OnUpdate(entt::registry& reg);
	private:
		// ���� �ý���
		void CheckGenerators(entt::registry& reg);
		void CheckPlayingPool();
	private:
		void Play(string sound_name, SoundType sound_type, bool looping, float volume, FXMVECTOR generate_pos);
		void CreateSoundPool();
		Sound* LoadSoundFromPool();
	};
}


