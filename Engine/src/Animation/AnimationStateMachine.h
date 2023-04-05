#pragma once
#include "AnimationState.h"
#include "Transition.h"

class AnimationStateMachine
{
public:
	void Update();

protected:
	std::unordered_map<int, AnimationState*> states_;
	std::multimap<int, Transition> transitions_;
	AnimationState* cur_state_ = nullptr;
};

