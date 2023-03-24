#pragma once

#include "SceneMgr.h"
#include "Character.h"

namespace reality {
	enum EVENT_TYPE {
		MOVEMENT,
		DELETE_ACTOR
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
			auto c_capsule = SCENE_MGR->GetRegistry().try_get<C_CapsuleCollision>(character->GetEntityId());
			if (character == nullptr)
				return;
			
			for (auto& block_wall : character->blocking_walls_)
			{
				XMVECTOR blocking_vector = block_wall.end - block_wall.start;
				XMVECTOR start_to_cap = c_capsule->capsule.base - block_wall.start; start_to_cap.m128_f32[1] = 0.0f;
				XMVECTOR project = Vector3Project(blocking_vector, start_to_cap);

				XMVECTOR normal1 = XMVector3Normalize(blocking_vector);
				XMVECTOR normal2 = XMVector3Normalize(project);

				XMVECTOR cap_to_blocking = XMVector3Normalize(project - start_to_cap) * c_capsule->capsule.radius;

				XMVECTOR movement = movement_vector_;
				movement.m128_f32[1] = 0.0f;
				float dot = XMVectorGetX(XMVector3Dot(movement, blocking_vector));
				if (dot < 0)
				{
					blocking_vector *= -1.0f;
				}

				float dot2 = XMVectorGetX(XMVector3Dot(cap_to_blocking, movement_vector_));
				if (dot2 > 0)
					movement_vector_ = Vector3Project(blocking_vector, movement_vector_);
			}

			character->blocking_walls_.clear();
			XMMATRIX transform_matrix = character->GetTranformMatrix();
			XMMATRIX movement_matrix = XMMatrixTranslationFromVector(movement_vector_);
			transform_matrix *= movement_matrix;


			switch (character->movement_state_)
			{
				case MovementState::STAND_ON_FLOOR :
					transform_matrix.r[3].m128_f32[1] = character->floor_height;
					break;
				case MovementState::JUMP:
					break;
				case MovementState::GRAVITY_FALL:
					break;
			}

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
}
