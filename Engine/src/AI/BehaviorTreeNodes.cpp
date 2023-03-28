#pragma once
#include "stdafx.h"
#include "BehaviorTreeNodes.h"

namespace reality {
    void BehaviorNode::SetStatus(BehaviorStatus new_status)
    {
        std::lock_guard<std::mutex> lock(status_mutex_);
        status_ = new_status;
    }
    
    BehaviorStatus BehaviorNode::GetStatus()
    {
        std::lock_guard<std::mutex> lock(status_mutex_);
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
        // Use a promise to pass the result from the thread to the caller
        std::promise<BehaviorStatus> promise;

        std::thread t([this, &promise]() {
            BehaviorStatus result = Action();

            // Set the promise value to the result
            promise.set_value(result);
            });

        // Detach the thread to allow it to run in the background
        t.detach();

        // Store the future object to retrieve the result later
        future_status_ = promise.get_future();
    }
    BehaviorStatus ActionNode::GetStatus()
    {
        if (future_status_.valid()) {
            SetStatus(future_status_.get());
        }
        return status_;
    }
}