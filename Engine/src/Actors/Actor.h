#pragma once
#include "ComponentSystem.h"

namespace reality
{
	class DLL_API Actor
	{
	protected:
		XMMATRIX		transform_matrix_ = XMMatrixIdentity();
	public:
		entt::entity	entity_id_;
		TransformTree	transform_tree_;
		int				node_num_;
		entt::registry* reg_scene_;
		string tag;

		bool visible = true;

	public:
		virtual void OnInit(entt::registry& registry);
		virtual void OnUpdate();

	public: 
		entt::entity GetEntityId();
		virtual void ApplyMovement(XMMATRIX transform_matrix);
		XMMATRIX	 GetTransformMatrix();
	};
}