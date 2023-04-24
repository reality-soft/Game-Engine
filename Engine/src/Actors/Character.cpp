#include "stdafx.h"
#include "Character.h"
#include "TimeMgr.h"
#include "SceneMgr.h"

void reality::Character::OnInit(entt::registry& registry)
{
	Actor::OnInit(registry);
	registry.emplace_or_replace<C_Movement>(entity_id_, &transform_tree_);
	movement_component_ = registry.try_get<C_Movement>(entity_id_);
}

void reality::Character::OnUpdate()
{
	front_ = XMVector3Transform(XMVECTOR{ 0.0f, 0.0f, 1.0f, 0.0f }, rotation_);
	right_ = XMVector3Transform(XMVECTOR{ 1.0f, 0.0f, 0.0f, 0.0f }, rotation_);
	transform_tree_.root_node->Rotate(*reg_scene_, entity_id_, cur_position_, rotation_);
}

void reality::Character::CancelMovement()
{
	movement_component_->velocity = { 0.0f, 0.0f, 0.0f, 0.0f };
}

void reality::Character::SetPos(const XMVECTOR& position)
{
	cur_position_ = position;
	transform_tree_.root_node->Translate(*reg_scene_, entity_id_, XMMatrixTranslationFromVector(cur_position_));
}

XMMATRIX reality::Character::GetRotation()
{
	return rotation_;
}
