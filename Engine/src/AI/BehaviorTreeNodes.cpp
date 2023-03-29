#pragma once
#include "stdafx.h"
#include "BehaviorTreeNodes.h"

namespace reality {
    void BehaviorNode::SetStatus(BehaviorStatus new_status)
    {
        status_ = new_status;
    }
    
    BehaviorStatus BehaviorNode::GetStatus()
    {
        return status_;
    }

    void BehaviorNode::ResetNode()
    {
        SetStatus(BehaviorStatus::IDLE);
        executing_child_node_index_ = -1;

        for (auto child : children_) {

        }
    }

    void SequenceNode::Execute()
	{
        if (children_.size() == 0) {
            status_ = BehaviorStatus::SUCCESS;
            return;
        }
        status_ = BehaviorStatus::RUNNING;

        BehaviorStatus child_status = children_[executing_child_node_index_]->GetStatus();

        switch (child_status) {
        case BehaviorStatus::IDLE:
            children_[executing_child_node_index_]->Execute();
            break;
        case BehaviorStatus::FAILURE:
            status_ = BehaviorStatus::FAILURE;
            break;
        case BehaviorStatus::SUCCESS:
            if (executing_child_node_index_ == children_.size() - 1) {
                status_ = BehaviorStatus::SUCCESS;
            }
            else {
                executing_child_node_index_++;
            }
            break;
        }
	}

    void SelectorNode::Execute()
	{
        if (children_.size() == 0) {
            status_ = BehaviorStatus::SUCCESS;
            return;
        }
        status_ = BehaviorStatus::RUNNING;

        BehaviorStatus child_status = children_[executing_child_node_index_]->GetStatus();

        switch (child_status) {
        case BehaviorStatus::IDLE:
            children_[executing_child_node_index_]->Execute();
            break;
        case BehaviorStatus::FAILURE:
            if (executing_child_node_index_ == children_.size() - 1) {
                status_ = BehaviorStatus::FAILURE;
            }
            else {
                executing_child_node_index_++;
            }
            break;
        case BehaviorStatus::SUCCESS:
            status_ = BehaviorStatus::SUCCESS;
            break;
        }
	}

    void ActionNode::Execute()
    {
        BehaviorStatus result = Action();
        SetStatus(result);
    }
}