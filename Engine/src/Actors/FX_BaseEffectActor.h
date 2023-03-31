#pragma once
#include "Actor.h"

namespace reality
{
	class DLL_API FX_BaseEffectActor : public Actor
	{
	public:
		virtual void OnInit(entt::registry& registry) override;
	
	public:
		void Spawn(XMVECTOR pos, XMVECTOR rotation_q = XMQuaternionIdentity(), XMVECTOR scale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
	protected:
		void AddEffectComponent(string effect_name, float lifetime = -1.0f);
		void AddSoundGeneratorComponent();
		void AddSoundQueue(string sound_name, SoundType type, bool looping = false, float volume = 10.0f);
		void AddPointLightComponent(string pointlight_name, float lifetime = -1.0f);
		void AddSpotLightComponent(string spotlight_name, float lifetime = -1.0f);

	public:
		// Only Used for Tool
		void AddEffect(map<string, Emitter>& emitter_list);
		void ResetEmitter();
	};
}


