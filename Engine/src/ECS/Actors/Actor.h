#pragma once
#include "DllMacro.h"
#include "../Systems/ComponentSystem.h"
#include "QuadTreeMgr.h"

namespace reality
{
	class DLL_API Actor
	{
	protected:
		entt::entity	entity_id_;
		TransformTree	transform_tree_;
		AABBShape		collision_box_;
		int				node_num_;
	public:
		virtual void OnInit(entt::registry& registry, AABBShape collision_box);
		virtual void OnUpdate(entt::registry& registry);
	public: 
		entt::entity GetEntityId () {
			return entity_id_;
		}
	};
}