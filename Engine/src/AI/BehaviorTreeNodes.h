#pragma once
#include "DllMacro.h"
#include "stdafx.h"

namespace reality {
    enum class DLL_API BehaviorStatus
    {
        RUNNING, 
        SUCCESS, 
        FAILURE, 
        IDLE, 
        HALTED, 
        EXIT
    };

    class DLL_API BehaviorNode
    {
    public:
        BehaviorNode() {};
        BehaviorNode(const vector<shared_ptr<BehaviorNode>>& children) : children_(children) {}

    public:
        virtual void Execute() {};
        virtual void ResetNode();

    public:
        void                   SetStatus(BehaviorStatus);
        virtual BehaviorStatus GetStatus();

    public:
        template<typename BehaviorNodeType, typename... Args>
        void AddChild(Args&&...args);

    private:
        std::string name_;
       
    protected:
        BehaviorStatus status_ = BehaviorStatus::IDLE;
        int executing_child_node_index_ = 0;

    protected:
        std::vector<shared_ptr<BehaviorNode>> children_;

    private:
        std::mutex status_mutex_;
    };

    class DLL_API SelectorNode : public BehaviorNode
    {
    public:
        SelectorNode() {};
        SelectorNode(const vector<shared_ptr<BehaviorNode>>& children) : BehaviorNode(children) {}

    public:
        virtual void Execute() override;
    };

    class DLL_API SequenceNode : public BehaviorNode
    {
    public:
        SequenceNode() {};
        SequenceNode(const vector<shared_ptr<BehaviorNode>>& children) : BehaviorNode(children) {}

    public:
        virtual void Execute() override;

    private:
        std::vector<shared_ptr<BehaviorNode>> children_;
    };

    class DLL_API ActionNode : public BehaviorNode
    {
    public:
        virtual void Execute() override;

    public:
        virtual BehaviorStatus GetStatus() override;

    private:
        virtual BehaviorStatus Action() = 0;

    private:
        std::thread action_thread_;
        std::future<BehaviorStatus> future_status_;
    };

    template<typename BehaviorNodeType, typename ...Args>
    inline void BehaviorNode::AddChild(Args&&...args)
    {
        children_.push_back(make_shared<BehaviorNodeType>(args...));
    }
}