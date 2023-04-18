#pragma once
#include "AnimationStateMachine.h"

namespace reality {
    class DLL_API AnimationState
    {
    private:
        int id_;

    public:
        AnimationState(int id) : id_(id) {};

    public:
        virtual void Enter(AnimationStateMachine* animation) {}
        virtual void Exit(AnimationStateMachine* animation) {}
        virtual void OnUpdate(AnimationStateMachine* animation) {}

    public:
        int GetId() {
            return id_;
        }
    };
}