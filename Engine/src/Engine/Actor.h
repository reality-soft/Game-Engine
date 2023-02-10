#pragma once
#include "common.h"
#include "DllMacro.h"
#include "ComponentSystem.h"

namespace KGCA41B
{
	class DLL_API Actor
	{
	protected:
		entt::entity entity_id_;
		TransformTree transform_tree_;
	public:
		void OnInit(entt::registry& registry);
	};
}

