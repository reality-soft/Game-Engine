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

		auto* movement_component = reg.try_get<C_Movement>(entity_id);

		XMVECTOR jump_vector = XMVectorSet(0, 1, 0, 0) * movement_component->jump_scale * TM_DELTATIME;
		XMVECTOR gravity_vector = XMVectorSet(0, -1, 0, 0) * movement_component->gravity * TM_DELTATIME;

		movement_component->jump_scale -= movement_component->gravity;
		movement_component->jump_scale = max(movement_component->jump_scale, 0);

		movement_component->direction = XMVector3Normalize(movement_component->direction);
		XMVECTOR movement_vector = movement_component->direction * movement_component->speed * TM_DELTATIME;
		movement_vector += jump_vector + gravity_vector;
		



		if (!XMVector4Equal(movement_vector, XMVectorZero())) {
			EVENT->PushEvent<MovementEvent>(movement_vector, entity_id);
		}

		movement_component->direction = XMVectorZero();
	}
}
