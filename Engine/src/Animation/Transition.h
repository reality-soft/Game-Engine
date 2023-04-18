#pragma once
#include "stdafx.h"

namespace reality {
	class AnimationStateMachine;

	struct DLL_API Transition {
		Transition(int to_state_id, function<bool(const AnimationStateMachine* animatinon_state_machine)> condition)
			: to_state_id_(to_state_id),
			  condition_(condition)
		{

		}
		int to_state_id_;
		function<bool(const AnimationStateMachine* animatinon_state_machine)> condition_;
	};
}