#pragma once
#include "BehaviorTreeNodes.h"

namespace reality {
    class DLL_API BehaviorTree
    {
    public:
        template<typename BehaviorNodeType, typename... Args>
        void          SetRootNode(Args&&...);
        void          Update();
        BehaviorNode* GetRootNode();

    private:
        shared_ptr<BehaviorNode> root_ = nullptr;
    };

    template<typename BehaviorNodeType, typename ...Args>
    inline void BehaviorTree::SetRootNode(Args && ... args)
    {
        root_ = make_shared<BehaviorNodeType>(args...);
    }
}

