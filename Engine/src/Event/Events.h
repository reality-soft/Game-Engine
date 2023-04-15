#pragma once

#include "SceneMgr.h"
#include "Character.h"

namespace reality {
	enum EVENT_TYPE {
		MOVEMENT,
		DELETE_ACTOR,
		TRIGGER,
	};

	class DLL_API Event
	{

		EVENT_TYPE event_type_;
		
	public:
		Event() {};
		Event(EVENT_TYPE event_type) : event_type_(event_type) {};
		virtual void Process() {};
	};

	class DLL_API MovementEvent : public Event {
	public:
		MovementEvent(XMVECTOR movement_vector, entt::entity actor_id) : Event(MOVEMENT) {
			movement_vector_ = movement_vector;
			actor_id_ = actor_id;
		}

		virtual void Process() override {

			auto character = SCENE_MGR->GetActor<Character>(actor_id_);
			if (character == nullptr)
				return;

			auto c_capsule = SCENE_MGR->GetRegistry().try_get<C_CapsuleCollision>(character->GetEntityId());

			for (int i = 0; i < 4; ++i)
			{
				auto plane = character->blocking_planes_[i];

				if (Vector3Length(_XMVECTOR4(plane)) <= 0.1f)
					continue;

				XMVECTOR plane_normal = XMVectorSet(plane.x, plane.y, plane.z, 0);

				if (XMVectorGetX(XMVector3Dot(plane_normal, movement_vector_)) < 0.0f)
					movement_vector_ = VectorProjectPlane(movement_vector_, plane_normal);
			}
			

			XMMATRIX transform_matrix = character->GetTranformMatrix();
			XMMATRIX movement_matrix = XMMatrixTranslationFromVector(movement_vector_);
			transform_matrix *= movement_matrix;

			if (character->movement_state_ == MovementState::WALK)
				transform_matrix.r[3].m128_f32[1] = character->floor_position.y;

			character->ApplyMovement(transform_matrix);
		};
	private:
		XMVECTOR movement_vector_;
		entt::entity actor_id_;
	};


	class DLL_API DeleteActorEvent : public Event {
	public:
		DeleteActorEvent(entt::entity actor_id) : Event(DELETE_ACTOR) {
			actor_id_ = actor_id;
		}

		virtual void Process() override {
			SCENE_MGR->DestroyActor(actor_id_);
		};
	private:
		entt::entity actor_id_;
	};

	class DLL_API TriggerEvent : public Event {
	public:
		TriggerEvent(entt::entity target_actor, entt::entity trigger_actor, bool begin_or_end) : Event(TRIGGER) {
			target_actor_ = target_actor;
			trigger_actor_ = trigger_actor;
			begin_or_end_ = begin_or_end;
		}

		virtual void Process() override {
			if (begin_or_end_ == true) // Begin Overlap
			{

			}
			else // End Overlap
			{

			}
		};

	private:
		entt::entity target_actor_;
		entt::entity trigger_actor_;
		bool begin_or_end_;
	};
}
