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
			anim_slot.second->AnimationUpdate();
		}

		auto* socket_component = reg.try_get<C_Socket>(ent);
		if (socket_component != nullptr) {
			for (auto& socket_pair : socket_component->sockets) {
				auto& socket = socket_pair.second;
				socket.animation_matrix = animation_component->GetCurAnimMatirixOfBone(socket.bone_id);
			}
		}
	}
}