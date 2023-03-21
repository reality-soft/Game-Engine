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

void reality::Actor::ApplyMovement(XMMATRIX transform_matrix)
{
	transform_matrix_ = transform_matrix;
	transform_tree_.root_node->Translate(*reg_scene_, entity_id_, transform_matrix_);
}
