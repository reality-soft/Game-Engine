#include "stdafx.h"
#include "AnimationSystem.h"

void reality::AnimationSystem::OnCreate(entt::registry& reg)
{
}

void reality::AnimationSystem::OnUpdate(entt::registry& reg)
{
	auto view_anim = reg.view<C_Animation>();

	for (auto ent : view_anim)
	{
		auto* animation_component = reg.try_get<C_Animation>(ent);
		
		for (const auto& anim_slot : animation_component->anim_slots) {
			anim_slot.second.OnUpdate();
		}
	}
}