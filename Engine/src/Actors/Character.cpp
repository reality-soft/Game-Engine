#include "stdafx.h"
#include "Character.h"
#include "TimeMgr.h"
#include "SceneMgr.h"
void reality::Character::OnInit(entt::registry& registry)
{
	Actor::OnInit(registry);
	registry.emplace_or_replace<C_Movement>(entity_id_, &transform_tree_);
}

void reality::Character::OnUpdate()
{
	if (rotate_enable_)
	{
		front_ = XMVector3Transform(XMVECTOR{ 0.0f, 0.0f, 1.0f, 0.0f }, rotation_);
		right_ = XMVector3Transform(XMVECTOR{ 1.0f, 0.0f, 0.0f, 0.0f }, rotation_);
		transform_tree_.root_node->Rotate(*reg_scene_, entity_id_, cur_position_, rotation_);
	}
}

reality::C_Movement* reality::Character::GetMovementComponent()
{
	return reg_scene_->try_get<C_Movement>(entity_id_);
}

void reality::Character::CancelMovement()
{
	GetMovementComponent()->velocity.m128_f32[0] = 0;
	GetMovementComponent()->velocity.m128_f32[2] = 0;
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

reality::C_CapsuleCollision* reality::Character::GetCapsuleComponent()
{
	return reg_scene_->try_get<C_CapsuleCollision>(entity_id_);
}

