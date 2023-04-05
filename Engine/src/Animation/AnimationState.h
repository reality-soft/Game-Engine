#pragma once
class AnimationState
{
protected:
    string prev_anim_id;
    string anim_id_;
    float blend_time_;
public:
    virtual void Enter() {}
    virtual void Exit() {}
    virtual void Update() {}
};

