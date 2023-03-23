#pragma once
#include "DllMacro.h"
#include "stdafx.h"

namespace reality {
    enum class DLL_API BehaviorStatus
    {
        Success,
        Failure,
        Running
    };

    class DLL_API BehaviorNode
    {
    public:
        virtual BehaviorStatus Execute() = 0;
    };

    class DLL_API SelectorNode : public BehaviorNode
    {
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
        virtual BehaviorStatus Execute() override = 0;
    protected:
        std::coroutine_handle<> coroutine_;
    };


    template<typename BehaviorNodeType, typename ...Args>
    inline void SelectorNode::AddChild(Args && ...args)
    {
        children_.push_back(make_shared<BehaviorNodeType>(Args...));
    }

    template<typename BehaviorNodeType, typename ...Args>
    inline void SequenceNode::AddChild(Args && ...args)
    {
        children_.push_back(make_shared<BehaviorNodeType>(Args...));
    }
}