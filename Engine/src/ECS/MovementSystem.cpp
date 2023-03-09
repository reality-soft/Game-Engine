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

		XMVECTOR movement_vector = movement_component->direction * movement_component->speed * TM_DELTATIME;

		MovementEvent movement_event(movement_vector, entity_id);
		EVENT->PushEvent(movement_event);
	}
}
