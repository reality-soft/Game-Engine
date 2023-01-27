#pragma once
#include "System.h"

namespace KGCA41B
{
	class DLL_API InputSystem : public System
	{
	public:
		virtual void OnCreare(entt::registry reg);
		virtual void OnUpdate(entt::registry reg);
	};
}
