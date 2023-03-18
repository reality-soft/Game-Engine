#pragma once
#include "System.h"

namespace reality
{
	class DLL_API PhysicsSystem : public System
	{
		virtual void OnCreate(entt::registry& reg);
		virtual void OnUpdate(entt::registry& reg);
	};
}

