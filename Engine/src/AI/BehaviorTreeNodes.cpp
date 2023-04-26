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

    void BehaviorNode::ResetNodes()
    {
        SetStatus(BehaviorStatus::IDLE);
        executing_child_node_index_ = 0;

        for (const auto& child : children_) {
            child->ResetNodes();
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
        case BehaviorStatus::RUNNING:
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
        case BehaviorStatus::RUNNING:
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

    void IfElseIfNode::Execute()
    {
        if (children_.size() == 0) {
            status_ = BehaviorStatus::SUCCESS;
            return;
        }
        status_ = BehaviorStatus::RUNNING;
        
        for (int i = 0;i < children_.size();i++) {
            if (children_[i].first()) {
                executing_child_node_index_ = i;
                break;
            }
        }

        BehaviorStatus child_status = children_[executing_child_node_index_].second->GetStatus();

        switch (child_status) {
        case BehaviorStatus::IDLE:
        case BehaviorStatus::RUNNING:
            children_[executing_child_node_index_].second->Execute();
            break;
        case BehaviorStatus::FAILURE:
            status_ = BehaviorStatus::FAILURE;
            break;
        case BehaviorStatus::SUCCESS:
            status_ = BehaviorStatus::SUCCESS;
            break;
        }
    }

    void IfElseIfNode::ResetNodes()
    {
        SetStatus(BehaviorStatus::IDLE);
        executing_child_node_index_ = 0;

        for (const auto& child : children_) {
            child.second->ResetNodes();
        }
    }

    void RepeatNode::Execute()
    {
        if (children_.size() == 0) {
            status_ = BehaviorStatus::SUCCESS;
            return;
        }
        status_ = BehaviorStatus::RUNNING;

        BehaviorStatus child_status = children_[0]->GetStatus();

        switch (child_status) {
        case BehaviorStatus::IDLE:
        case BehaviorStatus::RUNNING:
            children_[executing_child_node_index_]->Execute();
            break;
        case BehaviorStatus::FAILURE:
            children_[0]->ResetNodes();
            children_[0]->Execute();
            break;
        case BehaviorStatus::SUCCESS:
            children_[0]->ResetNodes();
            children_[0]->Execute();
            cur_rep_++;
            if (max_rep_ > 0 && cur_rep_ >= max_rep_) {
                cur_rep_ = 0;
                status_ = BehaviorStatus::SUCCESS;
            }
            break;
        }
    }

    void InfiniteRepeatNode::Execute()
    {
        if (children_.size() == 0) {
            status_ = BehaviorStatus::SUCCESS;
            return;
        }
        status_ = BehaviorStatus::RUNNING;

        BehaviorStatus child_status = children_[0]->GetStatus();

        switch (child_status) {
        case BehaviorStatus::IDLE:
        case BehaviorStatus::RUNNING:
            children_[executing_child_node_index_]->Execute();
            break;
        case BehaviorStatus::FAILURE:
            children_[0]->ResetNodes();
            children_[0]->Execute();
            break;
        case BehaviorStatus::SUCCESS:
            children_[0]->ResetNodes();
            children_[0]->Execute();
            break;
        }
    }

    void ActionNode::Execute()
    {
        BehaviorStatus result = Action();
        SetStatus(result);
    }
}