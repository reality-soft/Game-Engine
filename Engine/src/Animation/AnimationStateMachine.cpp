#include "stdafx.h"
#include "AnimationStateMachine.h"

namespace reality {
    void AnimationStateMachine::OnUpdate()
    {
        cur_state_->OnUpdate();

        auto range = transitions_.equal_range(cur_state_->GetId());
        for (auto it = range.first; it != range.second; ++it)
        {
            if (it->second.condition())
            {
                cur_state_->Exit();
                cur_state_ = states_[it->second.to_state_id];
                cur_state_->Enter();
                break;
            }
        }
    }
}

