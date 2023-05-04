#include "stdafx.h"
#include "AnimationSystem.h"
#include "SceneMgr.h"

void reality::AnimationSystem::OnCreate(entt::registry& reg)
{
}

void reality::AnimationSystem::OnUpdate(entt::registry& reg)
{
	const auto& view_anim = reg.view<C_Animation>();

	for (auto ent : view_anim)
	{
		if (SCENE_MGR->GetActor<Actor>(ent)->visible) {
			auto* animation_component = reg.try_get<C_Animation>(ent);

			animation_component->OnUpdate();

			auto* socket_component = reg.try_get<C_Socket>(ent);
			if (socket_component != nullptr) {
				for (auto& socket_pair : socket_component->sockets) {
					auto& socket = socket_pair.second;
					socket.animation_matrix = animation_component->GetCurAnimMatirixOfBone(socket.bone_id);
				}
			}
		}
	}
}