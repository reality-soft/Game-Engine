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

}

void reality::Character::ApplyMovement(XMMATRIX movement_matrix)
{
	transform_matrix_ *= movement_matrix;

	//switch (capsule_callback.reaction)
	//{
	//case CapsuleCallback::FLOOR:
	//	FastenAtFloor();
	//	break;
	//case CapsuleCallback::WALL:
	//	FastenAtFloor();
	//	break;
	//}

	switch (movement_state_)
	{
	case MovementState::STAND_ON_FLOOR:
		FastenAtFloor();
		break;
	case MovementState::JUMP:
		break;
	case MovementState::GRAVITY_FALL:
		break;
	case MovementState::BLOCK_BY_WALL:
		break;
	}

	transform_tree_.root_node->Translate(*reg_scene_, entity_id_, transform_matrix_);
}

void reality::Character::FastenAtFloor()
{
	transform_matrix_.r[3].m128_f32[1] = floor_height;
}

void reality::Character::GravityFall(float _gravity)
{
	movement_component_->gravity += _gravity;
}