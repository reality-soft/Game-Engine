#pragma once
#include "ComponentSystem.h"

namespace reality
{
	class DLL_API Actor
	{
	protected:
		XMVECTOR		cur_position_ = { 0.0f, 0.0f, 0.0f, 0.0f };
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
		virtual void ApplyMovement(XMVECTOR trnaslation_vector);
		XMVECTOR	 GetCurPosition();
	};
}