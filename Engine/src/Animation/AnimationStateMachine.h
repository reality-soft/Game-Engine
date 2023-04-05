#pragma once
#include "AnimationState.h"
#include "Transition.h"
#include "AnimationBase.h"

namespace reality {
	class DLL_API AnimationStateMachine : public AnimationBase
	{
	public:
		virtual void OnUpdate() override;

	public:
		AnimationState* GetCurrentAnimatinoState() {
			return cur_state_;
		}

	protected:
		std::unordered_map<int, AnimationState*> states_;
		std::multimap<int, Transition> transitions_;
		AnimationState* cur_state_ = nullptr;
	};
}


