#include "stdafx.h"
#include "AnimationState.h"

namespace reality {
    void AnimationStateMachine::OnInit()
    {
    }
    void AnimationStateMachine::OnUpdate()
    {
        cur_state_->OnUpdate(this);

        auto range = transitions_.equal_range(cur_state_->GetId());
        for (auto it = range.first; it != range.second; ++it)
        {
            if (it->second.condition_(this))
            {
                cur_state_->Exit(this);
                cur_state_ = states_[it->second.to_state_id_];
                cur_state_->Enter(this);
                break;
            }
        }
    }
}

