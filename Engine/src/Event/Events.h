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

	class DLL_API CameraShakeEvent : public Event {
	public:
		CameraShakeEvent(entt::entity actor_id, float shake_time, float magnitude, float frequency)
		{
			actor_id_ = actor_id; 
			shake_time_ = shake_time;
			magnitude_ = magnitude;
			frequency_ = frequency;
		}

		virtual void Process() override {
			auto camera = SCENE_MGR->GetRegistry().try_get<C_Camera>(actor_id_);
			
			if (camera == nullptr)
				return;

			camera->is_shaking = true;
			camera->shaking_timer = 0.0f;
			camera->shake_time = shake_time_;
			camera->shake_magnitude = magnitude_;
			camera->shake_frequency = frequency_;
		}
	private:
		entt::entity actor_id_;
		float shake_time_;
		float magnitude_;
		float frequency_;
	};
}
