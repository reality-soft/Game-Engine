#pragma once
#include "DllMacro.h"
#include "stdafx.h"

namespace reality {
    enum class DLL_API BehaviorStatus
    {
        SUCCESS,
        FAILURE,
        RUNNING,
        INVALID
    };

    class DLL_API BehaviorNode
    {
    public:
        virtual BehaviorStatus Execute() = 0;
        virtual std::future<BehaviorStatus>* Future() { return nullptr; };
        BehaviorStatus status_ = BehaviorStatus::INVALID;
    };

    class DLL_API SelectorNode : public BehaviorNode
    {
    public:
        SelectorNode() {};
        SelectorNode(const vector<shared_ptr<BehaviorNode>>& children) : children_(children) {}

    public:
        template<typename BehaviorNodeType, typename... Args>
        void AddChild(Args&&...args);

    public:
        virtual BehaviorStatus Execute() override;

    private:
        std::vector<shared_ptr<BehaviorNode>> children_;
    };

    class DLL_API SequenceNode : public BehaviorNode
    {
    public:
        SequenceNode() {};
        SequenceNode(const vector<shared_ptr<BehaviorNode>>& children) : children_(children) {}
    public:
        template<typename BehaviorNodeType, typename... Args>
        void AddChild(Args&&...args);

    public:
        virtual BehaviorStatus Execute() override;

    private:
        std::vector<shared_ptr<BehaviorNode>> children_;
    };

    class DLL_API ActionNode : public BehaviorNode
    {
    public:
        using ExecuteFunction = std::function<BehaviorStatus()>;

        ActionNode(const ExecuteFunction& execute_function) : execute_function_(execute_function) {}
    public:
        BehaviorStatus Execute() override;
        virtual std::future<BehaviorStatus>* Future() override
        {
            return &future_;
        }

    private:
        ExecuteFunction execute_function_;
        std::future<BehaviorStatus> future_;
    };

    template<typename BehaviorNodeType, typename ...Args>
    inline void SelectorNode::AddChild(Args && ...args)
    {
        children_.push_back(make_shared<BehaviorNodeType>(Args...));
    }

    template<typename BehaviorNodeType, typename ...Args>
    inline void SequenceNode::AddChild(Args && ...args)
    {
        children_.push_back(make_shared<BehaviorNodeType>(std::forward<Args>(args)...));
    }
}