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

void reality::Character::ApplyMovement(XMMATRIX movement_matrix)
{
	transform_matrix_ *= movement_matrix;

	switch (capsule_callback.reaction)
	{
	case CapsuleCallback::FLOOR:
		FastenAtFloor(transform_matrix_);
		break;
	case CapsuleCallback::WALL:
		FastenAtFloor(transform_matrix_);
		break;
	case CapsuleCallback::NONE:
		break;
	}

	transform_tree_.root_node->Translate(*reg_scene_, entity_id_, transform_matrix_);
}

void reality::Character::FastenAtFloor(XMMATRIX& movement_matrix)
{
	movement_component_->gravity = XMVectorZero();
	movement_matrix.r[3].m128_f32[1] = capsule_callback.floor_pos.m128_f32[1];
}

void reality::Character::GravityFall(float _gravity)
{
	movement_component_->gravity += XMVectorSet(0, -1, 0, 0) * _gravity * TM_DELTATIME;
}
