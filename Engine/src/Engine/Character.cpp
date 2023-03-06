#include "stdafx.h"
#include "Character.h"

void KGCA41B::Character::CharacterInit(entt::registry& registry, AABBShape collision_box)
{
	entity_id_ = registry.create();
	registry.emplace_or_replace<C_Movement>(entity_id_, &transform_tree_);
	movement_component_ = registry.try_get<C_Movement>(entity_id_);

	OnInit(registry, collision_box);
}
