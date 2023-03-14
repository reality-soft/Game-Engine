#pragma once
#include "DllMacro.h"
#include "entt.hpp"
#include "SceneMgr.h"
#include "../ECS/Actors/Character.h"

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
			// check the map data

			// check the other character & npc data

			//movement_component->actor_transform_tree->root_node->ApplyMovement(reg, entity_id, translation_matrix);
			SCENE_MGR->GetActor<Actor>(actor_id_)->ApplyMovement(XMMatrixTranslationFromVector(movement_vector_));
		};
	private:
		XMVECTOR movement_vector_;
		entt::entity actor_id_;
	};

}
