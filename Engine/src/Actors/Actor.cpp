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

XMVECTOR reality::Actor::GetCurPosition()
{
	return cur_position_;
}

void reality::Actor::ApplyMovement(XMVECTOR translation_vector)
{
	cur_position_ = translation_vector;
	transform_tree_.root_node->Translate(*reg_scene_, entity_id_, XMMatrixTranslationFromVector(cur_position_));
}