#pragma once
#include "DllMacro.h"
#include "../Systems/ComponentSystem.h"
#include "QuadTreeMgr.h"

namespace reality
{
	class DLL_API Actor
	{
	protected:
		XMMATRIX		transform_matrix_ = XMMatrixIdentity();
		entt::entity	entity_id_;
		TransformTree	transform_tree_;
		AABBShape		collision_box_;
		int				node_num_;
		entt::registry* reg_scene_;
	public:
		virtual void OnInit(entt::registry& registry);
		virtual void OnUpdate();
	public: 
		entt::entity GetEntityId();
		void ApplyMovement(XMMATRIX movement_matrix);
	};
}