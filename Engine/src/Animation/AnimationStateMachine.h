#pragma once
#include "AnimationState.h"
#include "Transition.h"
#include "AnimationBase.h"

namespace reality {
	class DLL_API AnimationStateMachine : public AnimationBase
	{
	public:
		AnimationStateMachine(entt::entity owner_id) : owner_id_(owner_id) {}

	public:
		virtual void OnInit() override;
		virtual void OnUpdate() override;

	public:
		entt::entity GetOwnerId() {
			return owner_id_;
		}

	public:
		AnimationState* GetCurrentAnimatinoState() {
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


