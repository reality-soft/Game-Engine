#pragma once
#include "DllMacro.h"
#include "ComponentSystem.h"
#include "QuadTreeMgr.h"

namespace KGCA41B
{
	class DLL_API Actor
	{
	protected:
		entt::entity	entity_id_;
		TransformTree	transform_tree_;
		AABBShape		collision_box_;
		int				node_num_;
	public:
		void OnInit(entt::registry& registry, AABBShape collision_box);
		void OnUpdate(entt::registry& registry);
	};
}