#pragma once
#include "Actor.h"

namespace reality
{
	class DLL_API FX_BaseEffectActor : public Actor
	{
	public:
		virtual void OnInit(entt::registry& registry) override;
		virtual void OnUpdate() override;
	};
}


