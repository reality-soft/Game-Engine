#include "stdafx.h"
#include "TimeMgr.h"
#include "QuadTreeMgr.h"
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


		float cur_y_axis_speed = movement_component->velocity.m128_f32[1];

		movement_component->velocity.m128_f32[1] = 0.0f;
		movement_component->velocity.m128_f32[0] += movement_component->accelaration_vector[0] * movement_component->acceleration * TM_DELTATIME;
		movement_component->velocity.m128_f32[2] += movement_component->accelaration_vector[2] * movement_component->acceleration * TM_DELTATIME;

		// deceleration
		if (movement_component->accelaration_vector[0] == 0 && 0 <= movement_component->velocity.m128_f32[0]) {
			movement_component->velocity.m128_f32[0] -= movement_component->acceleration * 3 * TM_DELTATIME;
			movement_component->velocity.m128_f32[0] = max(0.0f, movement_component->velocity.m128_f32[0]);
		}
		if (movement_component->accelaration_vector[0] == 0 && 0 >= movement_component->velocity.m128_f32[0]) {
			movement_component->velocity.m128_f32[0] += movement_component->acceleration * 3 * TM_DELTATIME;
			movement_component->velocity.m128_f32[0] = min(0.0f, movement_component->velocity.m128_f32[0]);
		}
		if (movement_component->accelaration_vector[2] == 0 && 0 <= movement_component->velocity.m128_f32[2]) {
			movement_component->velocity.m128_f32[2] -= movement_component->acceleration * 3 * TM_DELTATIME;
			movement_component->velocity.m128_f32[2] = max(0.0f, movement_component->velocity.m128_f32[2]);
		}
		if (movement_component->accelaration_vector[2] == 0 && 0 >= movement_component->velocity.m128_f32[2]) {
			movement_component->velocity.m128_f32[2] += movement_component->acceleration * 3 * TM_DELTATIME;
			movement_component->velocity.m128_f32[2] = min(0.0f, movement_component->velocity.m128_f32[2]);
		}

		movement_component->speed = XMVector3Length(movement_component->velocity).m128_f32[0];
		if (movement_component->speed >= movement_component->max_speed) {
			movement_component->velocity = XMVector3Normalize(movement_component->velocity);
			movement_component->velocity *= movement_component->max_speed;
			movement_component->speed = movement_component->max_speed;
		}

		cur_y_axis_speed += movement_component->jump_pulse - movement_component->gravity_pulse * TM_DELTATIME;
		movement_component->velocity.m128_f32[1] = cur_y_axis_speed;
		XMVECTOR cur_velocity = XMVector3Transform(movement_component->velocity, character->GetRotation());


		for (int i = 0; i < 4; ++i)
		{
			auto plane = character->blocking_planes_[i];

			if (Vector3Length(_XMVECTOR4(plane)) <= 0.1f)
				continue;

			XMVECTOR plane_normal = XMVectorSet(plane.x, plane.y, plane.z, 0);

			if (XMVectorGetX(XMVector3Dot(plane_normal, cur_velocity)) < 0.0f)
				cur_velocity = VectorProjectPlane(cur_velocity, plane_normal);
		}
		for (const auto& block_ray : QUADTREE->blocking_fields_)
		{
			RayShape ray = block_ray;
			ray.start.y = 0.0f;
			ray.end.y = 0.0f;
			XMVECTOR position = _XMVECTOR3(c_capsule->capsule.base); position.m128_f32[1] = 0.0f;
			XMVECTOR segment = PointRaySegment(ray, position);
			float distance = Distance(position, segment);
			//float distance = XMVector3LinePointDistance(line_start, line_end, position).m128_f32[0];
			if (distance <= c_capsule->capsule.radius)
			{
				XMVECTOR plane_normal = -1.0f * XMPlaneFromPoints(_XMVECTOR3(ray.start), _XMVECTOR3(ray.start) + XMVectorSet(0, 10000, 0, 0), _XMVECTOR3(ray.end));
				plane_normal.m128_f32[3] = 0.0f;

				if (XMVectorGetX(XMVector3Dot(plane_normal, cur_velocity)) < 0.0f)
					cur_velocity = VectorProjectPlane(cur_velocity, plane_normal);
			}
		}

		XMVECTOR character_cur_postion = character->GetCurPosition();
		character_cur_postion += cur_velocity * TM_DELTATIME;

		if (movement_component->jump_pulse <= 0)
		{
			if (character->movement_state_ == MovementState::WALK)
				character_cur_postion.m128_f32[1] = character->floor_position.y;
		}

		character->ApplyMovement(character_cur_postion);

		movement_component->accelaration_vector[0] = 0;
		movement_component->accelaration_vector[1] = 0;
		movement_component->accelaration_vector[2] = 0;
		movement_component->jump_pulse = 0;
	}
}
