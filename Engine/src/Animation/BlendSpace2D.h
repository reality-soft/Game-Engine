#pragma once
#include "AnimationBase.h"

namespace reality {
#define BLENDSPACE2D_ROW 50
#define BLENDSPACE2D_COL 50

    struct DLL_API AnimationClip {
    public:
        AnimationClip() {};
        AnimationClip(float x, float y, string animation_id);

    public:
        float x_;
        float y_;

    public:
        string animation_id_;
    };

    class BlendSpace2D : public AnimationBase {
    public:
        BlendSpace2D(const float& x, const float& y, float x_min, float x_max, float y_min, float y_max, entt::entity owner_id, string skeletal_mesh_id, int range, string bone_name = "")
            : x_(x), y_(y), x_min_(x_min), x_max_(x_max), y_min_(y_min), y_max_(y_max), owner_id_(owner_id),
            AnimationBase(skeletal_mesh_id, range, bone_name)
        {
            animation_clips_.resize(BLENDSPACE2D_ROW * BLENDSPACE2D_COL);
        }

    protected:
        void OnUpdate() override;

    public:
        AnimationClip GetClip(int row, int col) { return animation_clips_[row * cols_ + col]; }
        void AddClip(float x, float y, string animation_id);

    private:
        std::vector<AnimationClip> animation_clips_;
        const float& x_;
        const float& y_;
        int rows_;
        int cols_;
        float x_min_;
        float x_max_;
        float y_min_;
        float y_max_;
        entt::entity owner_id_;
    };
}