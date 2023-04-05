#pragma once
class AnimationState
{
private:
    int id_;

public:
    virtual void Enter() {}
    virtual void Exit() {}
    virtual void Update() {}

public:
    int GetId() {
        return id_;
    }
};

