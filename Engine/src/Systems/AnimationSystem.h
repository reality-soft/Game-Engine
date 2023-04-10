#pragma once
#include "System.h"

namespace reality {
	class DLL_API AnimationSystem : public System
	{
	public:
		virtual void OnCreate(entt::registry& reg) override;
		virtual void OnUpdate(entt::registry& reg) override;
	};

}

