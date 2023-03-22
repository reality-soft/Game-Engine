#pragma once
#include "FX_BaseEffectActor.h"

namespace reality
{
	class FX_ConcreteImpact : public FX_BaseEffectActor
	{
	public:
		virtual void OnInit(entt::registry& registry);
		virtual void OnUpdate();
	};
}

