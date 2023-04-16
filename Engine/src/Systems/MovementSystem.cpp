#include "stdafx.h"
#include "TimeMgr.h"
#include "MovementSystem.h"
#include "EventMgr.h"

void reality::MovementSystem::OnCreate(entt::registry& reg)
{
}

void reality::MovementSystem::OnUpdate(entt::registry& reg)  
{
	auto view_movement = reg.view<C_Movement>();

	for (auto entity_id : view_movement)
	{
		auto character = SCENE_MGR->GetActor<Character>(entity_id);
		auto movement_component = character->GetMovementComponent();
		auto c_capsule = SCENE_MGR->GetRegistry().try_get<C_CapsuleCollision>(character->GetEntityId());

		if (character == nullptr || movement_component == nullptr || c_capsule == nullptr)
			return;

		movement_component->direction.m128_f32[1] = 0.0f;
		movement_component->direction = XMVector3Normalize(movement_component->direction);
		XMVECTOR movement_vector = movement_component->direction * movement_component->speed * TM_DELTATIME;

		XMVECTOR jump_vector    = XMVectorSet(0,  1, 0, 0) * movement_component->jump_pulse    * TM_DELTATIME;
		XMVECTOR gravity_vector = XMVectorSet(0, -1, 0, 0) * movement_component->gravity_pulse * TM_DELTATIME;

		movement_vector += jump_vector + gravity_vector;
		movement_component->gravity_pulse = min(movement_component->gravity_pulse, 981.0f);
	
		if (movement_component->gravity_pulse >= movement_component->jump_pulse && movement_component->jump_pulse > 0.001f)
		{
			movement_component->jump_pulse = 0.0f;
			movement_component->gravity_pulse = 0.0f;
		}	

		for (int i = 0; i < 4; ++i)
		{
			auto plane = character->blocking_planes_[i];

			if (Vector3Length(_XMVECTOR4(plane)) <= 0.1f)
				continue;

			XMVECTOR plane_normal = XMVectorSet(plane.x, plane.y, plane.z, 0);

			if (XMVectorGetX(XMVector3Dot(plane_normal, movement_vector)) < 0.0f)
				movement_vector = VectorProjectPlane(movement_vector, plane_normal);
		}


		XMMATRIX transform_matrix = character->GetTranformMatrix();
		XMMATRIX movement_matrix = XMMatrixTranslationFromVector(movement_vector);
		transform_matrix *= movement_matrix;

		if (movement_component->jump_pulse <= 0)
		{
			if (character->movement_state_ == MovementState::WALK)
				transform_matrix.r[3].m128_f32[1] = character->floor_position.y;
		}


		character->ApplyMovement(transform_matrix);

		movement_component->direction = XMVectorZero();
	}
}
