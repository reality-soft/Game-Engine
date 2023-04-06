#include "stdafx.h"
#include "AnimationStateMachine.h"

namespace reality {
    void AnimationStateMachine::OnUpdate()
    {
        cur_state_->OnUpdate(animation_);

        auto range = transitions_.equal_range(cur_state_->GetId());
        for (auto it = range.first; it != range.second; ++it)
        {
            if (it->second.condition(animation_))
            {
                cur_state_->Exit(animation_);
                cur_state_ = states_[it->second.to_state_id];
                cur_state_->Enter(animation_);
                break;
            }
        }
    }
}

