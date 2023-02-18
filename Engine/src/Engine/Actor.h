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
		int				node_num_;
	public:
		virtual void OnInit(entt::registry& registry);
		virtual void OnUpdate(entt::registry& registry);
	};
}

