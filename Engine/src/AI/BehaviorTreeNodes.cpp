#pragma once
#include "stdafx.h"
#include "BehaviorTreeNodes.h"

namespace reality {
	BehaviorStatus SequenceNode::Execute()
	{
        for (auto child : children_)
        {
            auto result = child->Execute();
            if (result != BehaviorStatus::Success)
            {
                return result;
            }
        }
        return BehaviorStatus::Success;
	}

	BehaviorStatus SelectorNode::Execute()
	{
        for (auto child : children_)
        {
            auto result = child->Execute();
            if (result == BehaviorStatus::Success)
            {
                return BehaviorStatus::Success;
            }
        }
        return BehaviorStatus::Failure;
	}
}