#pragma once
#include "FX_BaseEffectActor.h"

namespace reality {
	class DLL_API FX_Effect : public FX_BaseEffectActor
	{
	public:
		virtual void OnInit(entt::registry& registry, string effect_id = "");
		virtual void OnUpdate() override;
	public:
		void AddEffect(map<string, Emitter>& emitter_list);
		void ResetEmitter();
	};
}