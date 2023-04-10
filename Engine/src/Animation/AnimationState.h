#pragma once
#include "AnimationBase.h"

namespace reality {
    class DLL_API AnimationState
    {
    private:
        int id_;

    public:
        AnimationState(int id) : id_(id) {};

    public:
        virtual void Enter(AnimationBase* animation) {}
        virtual void Exit(AnimationBase* animation) {}
        virtual void OnUpdate(AnimationBase* animation) {}

    public:
        int GetId() {
            return id_;
        }
    };
}