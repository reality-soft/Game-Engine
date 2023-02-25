#pragma once
#include "DllMacro.h"
#include "ComponentSystem.h"
#include "QuadTreeMgr.h"

namespace KGCA41B
{
	class DLL_API StaticObject
	{
	protected:
		entt::entity	entity_id_;
		TransformTree	transform_tree_;
		int				node_num_;
	public:
		virtual void OnInit(entt::registry& registry, KGCA41B::C_Transform transform, AABBShape collision_box, string static_mesh_id);
		virtual void OnUpdate(entt::registry& registry);
		entt::entity GetEntity() { return entity_id_; }

	public:
		XMMATRIX transform;

		void SetTransformTRS(const XMVECTOR& t, const XMVECTOR& r, const XMVECTOR& s);
	};
}

