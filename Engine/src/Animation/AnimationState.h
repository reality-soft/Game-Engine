#pragma once

namespace reality {
    class DLL_API AnimationState
    {
    private:
        int id_;

    protected:
        string prev_anim_id_;
        string post_anim_id_;
        float blend_time_;
        float cur_frame = 0.0f;

    public:
        AnimationState(int id) : id_(id) {};

    public:
        virtual void Enter() {}
        virtual void Exit() {}
        virtual void OnUpdate() {}

    public:
        int GetId() {
            return id_;
        }
    };
}