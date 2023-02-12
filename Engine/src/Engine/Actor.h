#pragma once
#include "stdafx.h"
#include "DllMacro.h"
#include "ComponentSystem.h"

namespace KGCA41B
{
	class DLL_API Actor
	{
	protected:
		entt::entity	entity_id_;
		TransformTree	transform_tree_;
		AABB<3>			collision_box_;
		int				node_num_;
	public:
		void OnInit(entt::registry& registry, AABB<3> collision_box);
		void OnUpdate(entt::registry& registry);
	};
}

