#pragma once
#include "Animation.h"

namespace reality {
    class DLL_API AnimationState
    {
    private:
        int id_;

    public:
        AnimationState(int id) : id_(id) {};

    public:
        virtual void Enter(Animation& animation) {}
        virtual void Exit(Animation& animation) {}
        virtual void OnUpdate(Animation& animation) {}

    public:
        int GetId() {
            return id_;
        }
    };
}