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

void reality::Character::GravityFall(float _gravity)
{
	movement_component_->gravity_pulse += _gravity;
}

XMVECTOR reality::Character::GetPos() const
{
	XMVECTOR translation;
	translation.m128_f32[0] = transform_matrix_.r[3].m128_f32[0];
	translation.m128_f32[1] = transform_matrix_.r[3].m128_f32[1];
	translation.m128_f32[2] = transform_matrix_.r[3].m128_f32[2];
	translation.m128_f32[3] = 0.0f;

	return translation;
}

void reality::Character::SetPos(const XMVECTOR& position)
{
	transform_matrix_ = XMMatrixTranslationFromVector(position);
	transform_tree_.root_node->Translate(*reg_scene_, entity_id_, transform_matrix_);
}

void reality::Character::RotateAlongMovementDirection()
{
	XMVECTOR scale, rotation, translation;
	XMMatrixDecompose(&scale, &rotation, &translation, transform_matrix_);

	XMVECTOR movement_direction = movement_component_->direction;
	movement_direction.m128_f32[1] = 0.0f;
	movement_direction = XMVector3Normalize(movement_direction);
	float dot_product = XMVectorGetX(XMVector3Dot(movement_component_->direction, { 0, 0, 1, 0 }));
	float magnitude1 = XMVectorGetX(XMVector3Length(movement_component_->direction));
	float magnitude2 = XMVectorGetX(XMVector3Length({ 0, 0, 1, 0 }));

	float radian_angle = acos(dot_product / (magnitude1 * magnitude2));

	// Az * Bx - Ax * Bz < 0
	if (-movement_direction.m128_f32[2] < 0)
	{
		radian_angle = 2 * M_PI - radian_angle;
	}

	XMMATRIX rotation_matrix = XMMatrixRotationY(radian_angle);
	transform_tree_.root_node->Rotate(*reg_scene_, entity_id_, translation, rotation_matrix);
	front_ = XMVector3Transform({ 0, 0, 1, 0 }, rotation_matrix);
	right_ = XMVector3Transform({ 1, 0, 0, 0 }, rotation_matrix);
}
