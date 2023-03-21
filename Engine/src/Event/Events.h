#pragma once

#include "SceneMgr.h"
#include "Character.h"

namespace reality {
	enum EVENT_TYPE {
		MOVEMENT
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
			
			for (auto& block_vector : character->blocking_vector_list)
			{

					XMVECTOR cross = -XMVector3Normalize(XMVector3Cross(block_vector, XMVectorSet(0, 1, 0, 0)));

					float dot = XMVectorGetX(XMVector3Dot(movement_vector_, block_vector));
					if (dot < 0)
					{
						block_vector *= -1.0f;
					}

					XMVECTOR line_point = cross * c_capsule->capsule.radius;

					if (XMVectorGetX(XMVector3Dot(line_point, movement_vector_)) > 0)
						movement_vector_ = Vector3Project(block_vector, movement_vector_);
			}


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

}
