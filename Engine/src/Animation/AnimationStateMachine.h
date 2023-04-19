#pragma once
#include "Transition.h"
#include "AnimationBase.h"

namespace reality {
	class AnimationState;

	class DLL_API AnimationStateMachine : public AnimationBase
	{
	public:
		AnimationStateMachine(entt::entity owner_id, string skeletal_mesh_id, string bone_name, int range, int num_of_bones) : 
			owner_id_(owner_id), 
			AnimationBase(skeletal_mesh_id, bone_name, range, num_of_bones) {}

	public:
		virtual void OnInit() override;
		virtual void OnUpdate() override;

	public:
		entt::entity GetOwnerId() {
			return owner_id_;
		}

	public:
		AnimationState* GetCurrentAnimationState() {
			return cur_state_.get();
		}

	protected:
		std::unordered_map<int, shared_ptr<AnimationState>> states_;
		std::multimap<int, Transition> transitions_;
		shared_ptr<AnimationState> cur_state_ = nullptr;

	protected:
		entt::entity owner_id_;
	};
}


