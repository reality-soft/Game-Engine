#pragma once
#include "System.h"
#include "PhysicsMgr.h"

namespace reality
{
	class DLL_API PhysicsSystem : public System
	{
		virtual void OnCreate(entt::registry& reg);
		virtual void OnUpdate(entt::registry& reg);
	};
}

