#pragma once
#include "System.h"
#include "InputMgr.h"

namespace KGCA41B
{
	class DLL_API InputSystem : public System
	{
	public:
		virtual void OnCreate(entt::registry& reg);
		virtual void OnUpdate(entt::registry& reg);
		void Clear(InputMapping& input_mapping);
	};
}
