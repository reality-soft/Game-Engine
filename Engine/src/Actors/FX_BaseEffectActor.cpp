#include "stdafx.h"
#include "FX_BaseEffectActor.h"

void reality::FX_BaseEffectActor::OnInit(entt::registry& registry)
{
	Actor::OnInit(registry);
	

	C_Transform transform;
	transform.world = XMMatrixIdentity();
	transform.local = XMMatrixIdentity();
	registry.emplace<C_Transform>(entity_id_, transform);

	transform_tree_.root_node = make_shared<TransformTreeNode>(TYPE_ID(reality::C_Transform));

	transform_tree_.root_node->OnUpdate(registry, entity_id_);
}

void reality::FX_BaseEffectActor::OnUpdate()
{
	//this->node_num_ = SpacePartition::GetInst()->UpdateNodeObjectBelongs(0, collision_box_, entity_id_);
	//vector<int> node_to_search = SpacePartition::GetInst()->FindCollisionSearchNode(0, collision_box_);
	transform_tree_.root_node->OnUpdate(*reg_scene_, entity_id_);
}
