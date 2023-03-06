#include "stdafx.h"
#include "TimeMgr.h"
#include "MovementSystem.h"

void KGCA41B::MovementSystem::OnCreate(entt::registry& reg)
{
}

void KGCA41B::MovementSystem::OnUpdate(entt::registry& reg)
{
	auto view_movement = reg.view<C_Movement>();

	for (auto entity_id : view_movement)
	{
		auto* movement_component = reg.try_get<C_Movement>(entity_id);

		XMMATRIX translation_matrix = XMMatrixTranslationFromVector(movement_component->direction * movement_component->speed * TM_DELTATIME);

		movement_component->actor_transform_tree->root_node->ApplyMovement(reg, entity_id, translation_matrix);
	}
}
