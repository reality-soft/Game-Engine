#pragma once
#include "System.h"

#define SOUND_PATH L"D:/Sound/"
#define CHANNEL_MAX_COUNT 1024
#define POOL_SIZE 10

namespace KGCA41B {

	struct Sound
	{
		wstring sound_filename;
		FMOD::Channel* channel;
		FMOD::Sound* sound;
		UINT total_time;
		UINT current_time;
	public:
		Sound() : sound_filename(L""), channel(nullptr), sound(nullptr), total_time(0), current_time(0){ }
		Sound(Sound& sound) = default; 
	};

	class DLL_API SoundSystem : public System
	{
	private:
		// 리소스 매니저
		wstring sound_directory_address_;
		FMOD::System* fmod_system_;
	private:
		// 사운드 시스템
		FMOD::ChannelGroup* sfx_channel_group_;
		FMOD::ChannelGroup* music_channel_group_;
	private:
		// 리소스 매니저
		map<wstring, FMOD::Sound*>		sound_resource_list_;
		// 사운드 시스템
		list<Sound*>			sound_play_list;
		queue<Sound*>			sound_pool;
	public:
		virtual void OnCreate(entt::registry& reg);
		virtual void OnUpdate(entt::registry& reg);
		void OnRelease();
	private:
		// 사운드 시스템
		void CheckGenerators(entt::registry& reg);
		void CheckPlayingPool();
	private:
		// 리소스 매니저
		void CreateFmodSystem();
		void CreateFmodChannelGroup();
		void LoadDir(wstring directory_address);
		void LoadFile(wstring file_address);
		// 사운드 시스템
		void Play(wstring sound_name, SoundType sound_type, bool looping, float volume, FXMVECTOR generate_pos);
		void CreateSoundPool();
		Sound* LoadSoundFromPool();
	};
}


