#pragma once
#include "System.h"

#define SOUND_PATH L"D:/Sound/"
#define POOL_SIZE 10

namespace KGCA41B {

	struct SoundData
	{
		wstring sound_filename;
		FMOD::Channel* channel;
		FMOD::Sound* sound;
		UINT total_time;
		UINT current_time;
	public:
		SoundData() : sound_filename(L""), channel(nullptr), sound(nullptr), total_time(0), current_time(0)
		{

		}
		~SoundData()
		{
			sound->release();
		}
	};

	class DLL_API SoundSystem : public System
	{
	private:
		wstring sound_directory_address_;
		FMOD::System* fmod_system_;
	private:
		map<wstring, SoundData*>			sound_resource_list_;
		map<wstring, queue<SoundData*>>		sound_pool_wait;
		list<SoundData*>					sound_pool_play;
	public:
		virtual void OnCreate(entt::registry& reg);
		virtual void OnUpdate(entt::registry& reg);
		bool OnRelease();
	public:
		void CheckGenerators(entt::registry& reg);
		void CheckPlayingPool();
	public:
		void Play(wstring sound_name, bool looping, float volume, FXMVECTOR generate_pos);
		void LoadDir(wstring directory_address);
		void LoadFile(wstring file_address);
		void MakeSoundPool();
	};
}


