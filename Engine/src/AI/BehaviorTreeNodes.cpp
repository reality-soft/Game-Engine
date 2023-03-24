#pragma once
#include "stdafx.h"
#include "BehaviorTreeNodes.h"

namespace reality {
	BehaviorStatus SequenceNode::Execute()
	{
        for (auto child : children_)
        {
            BehaviorStatus result;
            if (child->Future()->_Get_value() != BehaviorStatus::INVALID) {
                result = child->Execute();
            }
            if (result != BehaviorStatus::SUCCESS)
            {
                return result;
            }
        }
        return BehaviorStatus::SUCCESS;
	}

	BehaviorStatus SelectorNode::Execute()
	{
        for (auto child : children_)
        {
            auto result = child->Execute();
            if (result != BehaviorStatus::FAILURE)
            {
                return result;
            }
        }
        return BehaviorStatus::FAILURE;
	}

    BehaviorStatus ActionNode::Execute()
    {
        if (!future_.valid()) {
            future_ = std::async(std::launch::async, execute_function_);
        }

        if (future_.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            status_ = future_.get();
            return status_;
        }

        return BehaviorStatus::RUNNING;
    }
}