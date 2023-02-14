#pragma once
#include "DllMacro.h"
#include "ComponentSystem.h"
#include "SpacePartition/SpacePartition.h"

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
		virtual void OnInit(entt::registry& registry, AABB<3> collision_box);
		virtual void OnUpdate(entt::registry& registry);
	};
}

