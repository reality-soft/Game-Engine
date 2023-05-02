#pragma once
#include "System.h"


namespace reality {
	class DLL_API SoundSystem : public System
	{
	public:
		virtual void OnCreate(entt::registry& reg) {}
		virtual void OnUpdate(entt::registry& reg);
    
	private:
		void CheckGenerators(entt::registry& reg);
	public:
		void PlayBackground(string sound_name, bool looping, float fade_in, float volume);
		bool FadeOutDelete(string sound_name, float fade_out);
	};
}


