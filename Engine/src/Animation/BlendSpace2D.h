#pragma once
#include "AnimationBase.h"

namespace reality {
#define BLENDSPACE2D_ROW 50
#define BLENDSPACE2D_COL 50

    struct DLL_API AnimationClip {
    public:
        AnimationClip() {};
        AnimationClip(float x, float y, bool is_valid, string animation_id);

    public:
        float x_;
        float y_;

    public:
        bool is_valid_ = false;

    public:
        string animation_id_;
    };

    enum class BLEND_DIRECTION {
        H_X_H_Y,
        H_X_L_Y,
        L_X_H_Y,
        L_X_L_Y,
    };

    class DLL_API BlendSpace2D : public AnimationBase {
    public:
        BlendSpace2D(const float& x, const float& y, float x_min, float x_max, float y_min, float y_max, float start_frame, float end_frame, entt::entity owner_id, string skeletal_mesh_id, int range, string bone_name = "")
            : x_(x), y_(y), x_min_(x_min), x_max_(x_max), y_min_(y_min), y_max_(y_max), owner_id_(owner_id),
            AnimationBase(skeletal_mesh_id, range, bone_name)
        {
            animation_.start_frame_ = start_frame;
            animation_.end_frame_ = end_frame;
            cur_anim_state_ = ANIM_STATE::ANIM_STATE_CUR_ONLY;
        }

    protected:
        void OnUpdate() override;

    private:
        AnimationClip GetClip(int x, int y, BLEND_DIRECTION blend_direction);

    public:
        void AddClip(float x, float y, string animation_id);

    private:
        std::vector<AnimationClip> animation_clips_;
        const float& x_;
        const float& y_;
        float x_min_;
        float x_max_;
        float y_min_;
        float y_max_;
        entt::entity owner_id_;
    };
}