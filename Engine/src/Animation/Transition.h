#pragma once
#include "stdafx.h"

namespace reality {
	struct DLL_API Transition {
		Transition(int to_state_id, function<bool(const AnimationBase* animation_base)> condition)
			: to_state_id_(to_state_id),
			  condition_(condition)
		{

		}
		int to_state_id_;
		function<bool(const AnimationBase* animation_base)> condition_;
	};
}