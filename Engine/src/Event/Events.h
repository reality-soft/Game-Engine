#pragma once

#include "SceneMgr.h"
#include "Character.h"

namespace reality {
	class DLL_API Event
	{	
	public:
		Event() {};
		virtual void Process() {};
	};

	class DLL_API MovementEvent : public Event {
	public:
		MovementEvent(XMVECTOR movement_vector, entt::entity actor_id) {
			movement_vector_ = movement_vector;
			actor_id_ = actor_id;
		}

		virtual void Process() override {
		};
	private:
		XMVECTOR movement_vector_;
		entt::entity actor_id_;
	};

	class DLL_API DeleteActorEvent : public Event {
	public:
		DeleteActorEvent(entt::entity actor_id) {
			actor_id_ = actor_id;
		}

		virtual void Process() override {
			SCENE_MGR->DestroyActor(actor_id_);
		};
	private:
		entt::entity actor_id_;
	};
}
