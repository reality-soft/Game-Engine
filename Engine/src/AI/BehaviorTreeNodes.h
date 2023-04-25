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
        virtual void ResetNodes();

    public:
        void                   SetStatus(BehaviorStatus);
        virtual BehaviorStatus GetStatus();

    public:
        template<typename BehaviorNodeType, typename... Args>
        void AddChild(Args&&...args);

    private:
        std::string name_;

    protected:
        std::vector<shared_ptr<BehaviorNode>> children_;
       
    protected:
        BehaviorStatus status_ = BehaviorStatus::IDLE;
        int executing_child_node_index_ = 0;

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
    };

    class DLL_API IfElseIfNode : public BehaviorNode
    {
    public:
        IfElseIfNode() {};
        IfElseIfNode(const std::vector<std::pair<std::function<bool()>, shared_ptr<BehaviorNode>>>& children, shared_ptr<BehaviorNode> else_node = nullptr) : children_(children), else_node_(else_node){
            children_.push_back({ []() { return true; }, else_node });
        }

    public:
        virtual void Execute() override;
        virtual void ResetNodes() override;

    protected:
        std::vector<std::pair<std::function<bool()>, shared_ptr<BehaviorNode>>> children_;
        shared_ptr<BehaviorNode> else_node_;
    };

    class DLL_API ActionNode : public BehaviorNode
    {
    public:
        virtual void Execute() override;

    private:
        virtual BehaviorStatus Action() = 0;
    };

    template<typename BehaviorNodeType, typename ...Args>
    inline void BehaviorNode::AddChild(Args&&...args)
    {
        children_.push_back(make_shared<BehaviorNodeType>(args...));
    }
}