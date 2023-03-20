#pragma once
#include "Actor.h"

namespace reality
{
	class DLL_API FX_BaseEffectActor : public Actor
	{
	public:
		virtual void OnInit(entt::registry& registry) override;
		virtual void OnInit(entt::registry& registry, string effect_id);
		virtual void OnUpdate() override;
	public:
		void AddEffect(map<string, Emitter>& emitter_list);
		void ResetEmitter();
	};
}


