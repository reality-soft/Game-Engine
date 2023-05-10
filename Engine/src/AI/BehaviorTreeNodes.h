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
        BehaviorNode(const BehaviorNode& other) {
            for (auto& ptr : other.children_) {
                children_.push_back(ptr);
            }

            name_ = other.name_;
            status_ = other.status_;
            executing_child_node_index_ = other.executing_child_node_index_;
        }
        BehaviorNode& operator=(const BehaviorNode& other) {
            if (this != &other) {
                children_.clear();
                for (auto& ptr : other.children_) {
                    children_.push_back(ptr);
                }
            }
            name_ = other.name_;
            status_ = other.status_;
            executing_child_node_index_ = other.executing_child_node_index_;

            return *this;
        }
        BehaviorNode(const vector<shared_ptr<BehaviorNode>>& children) : children_(children) {}

    public:
        virtual void Execute() {};
        virtual void ResetNodes();

    public:
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
    };

    class DLL_API SelectorNode : public BehaviorNode
    {
    public:
        SelectorNode() {};
        SelectorNode(const vector<shared_ptr<BehaviorNode>>& children) : BehaviorNode(children) {}
        SelectorNode(const SelectorNode& other) : BehaviorNode(other) {}
        SelectorNode& operator=(const SelectorNode& other) {
            if (this != &other) {
                BehaviorNode::operator=(other);
            }
            return *this;
        }

    public:
        virtual void Execute() override;
    };

    class DLL_API RandomSelectorNode : public BehaviorNode
    {
    public:
        RandomSelectorNode() {
            executing_child_node_index_ = -1;
        };
        RandomSelectorNode(const SelectorNode& other) : BehaviorNode(other) {
            executing_child_node_index_ = -1;
        }
        RandomSelectorNode& operator=(const RandomSelectorNode& other) {
            if (this != &other) {
                BehaviorNode::operator=(other);
            }
            executing_child_node_index_ = -1;
            return *this;
        }

    public:
        virtual BehaviorStatus GetStatus() override { return status_; };

    public:
        void SetNumToExecute(int iter_num) {
            num_to_execute_.push_back(iter_num);
            cur_num_executed_.push_back(0);
        };

    public:
        virtual void Execute() override;
        virtual void ResetNodes() override;

    private:
        vector<int> num_to_execute_;
        vector<int> cur_num_executed_;
    };

    class DLL_API SequenceNode : public BehaviorNode
    {
    public:
        SequenceNode() {};
        SequenceNode(const vector<shared_ptr<BehaviorNode>>& children) : BehaviorNode(children) {}
        SequenceNode(const SequenceNode& other) : BehaviorNode(other) {}
        SequenceNode& operator=(const SequenceNode& other) {
            if (this != &other) {
                BehaviorNode::operator=(other);
            }
            return *this;
        }

    public:
        virtual void Execute() override;
    };

    class DLL_API RepeatNode : public BehaviorNode {
    public:
        RepeatNode(shared_ptr<BehaviorNode> child_node, int maxRepetitions = -1) : BehaviorNode({ child_node }), max_rep_(maxRepetitions) {}
        RepeatNode(const RepeatNode& other) : BehaviorNode(other) {}
        RepeatNode& operator=(const RepeatNode& other) {
            if (this != &other) {
                BehaviorNode::operator=(other);
            }

            max_rep_ = other.max_rep_;
            cur_rep_ = other.cur_rep_;

            return *this;
        }

        virtual void Execute() override;
    private:
        int max_rep_;
        int cur_rep_;
    };

    class DLL_API InfiniteRepeatNode : public BehaviorNode {
    public:
        InfiniteRepeatNode(shared_ptr<BehaviorNode> child_node) : BehaviorNode({ child_node }) {}
        InfiniteRepeatNode(const InfiniteRepeatNode& other) : BehaviorNode(other) {}
        InfiniteRepeatNode& operator=(const InfiniteRepeatNode& other) {
            if (this != &other) {
                BehaviorNode::operator=(other);
            }

            return *this;
        }

        virtual void Execute() override;
    };


    class DLL_API IfElseIfNode : public BehaviorNode
    {
    public:
        IfElseIfNode() {};
        IfElseIfNode(const std::vector<std::pair<std::function<bool()>, shared_ptr<BehaviorNode>>>& children, shared_ptr<BehaviorNode> else_node = nullptr) : children_(children) {
            children_.push_back({ []() { return true; }, else_node });
        }
        IfElseIfNode(const IfElseIfNode& other) {
            for (auto& ptr : other.children_) {
                children_.push_back(ptr);
            }
        }
        IfElseIfNode& operator=(const IfElseIfNode& other) {
            for (auto& ptr : other.children_) {
                children_.push_back(ptr);
            }

            return *this;
        }

    public:
        virtual void Execute() override;
        virtual void ResetNodes() override;

    protected:
        std::vector<std::pair<std::function<bool()>, shared_ptr<BehaviorNode>>> children_;
    };

    class DLL_API ActionNode : public BehaviorNode
    {
    public:
        ActionNode() {};
        ActionNode(const ActionNode& other) : BehaviorNode(other) {}
        ActionNode& operator=(const ActionNode& other) {
            if (this != &other) {
                BehaviorNode::operator=(other);
            }

            return *this;
        }

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