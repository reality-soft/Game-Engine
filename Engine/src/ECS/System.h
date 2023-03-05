#pragma once
#include "Components.h"

namespace reality
{
	class System
	{
	public:
		System() = default;
		~System() = default;

		virtual void OnCreate(entt::registry& reg) = 0;
		virtual void OnUpdate(entt::registry& reg) = 0;
	};
}