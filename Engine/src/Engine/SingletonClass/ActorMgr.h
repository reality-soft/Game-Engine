#pragma once
#include "stdafx.h"
#include "Actor.h"

namespace reality {
	class ActorMgr
	{
		SINGLETON(ActorMgr)
#define ACTOR ActorMgr::GetInst()
		unordered_map<entt::entity, shared_ptr<Actor>> actor_map_;

		void AddActor(shared_ptr<Actor> actor_ptr) {
			actor_map_.insert({ actor_ptr->GetEntityId(), move(actor_ptr) });
		}
		weak_ptr<Actor> GetActor(entt::entity actor_id) {
			weak_ptr<Actor> actor_weak_ptr(actor_map_[actor_id]);
			return actor_weak_ptr;
		}
	};
}

