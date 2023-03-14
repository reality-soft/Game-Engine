#include "stdafx.h"
#include "Actor.h"

void reality::Actor::OnInit(entt::registry& registry)
{
	entity_id_ = registry.create();
	reg_scene_ = &registry;
}

void reality::Actor::OnUpdate()
{

}

entt::entity reality::Actor::GetEntityId()
{
	return entity_id_;
}

void reality::Actor::ApplyMovement(XMMATRIX movement_matrix)
{
	transform_matrix_ *= movement_matrix;
	transform_tree_.root_node->ApplyMovement(*reg_scene_, entity_id_, transform_matrix_);
}
