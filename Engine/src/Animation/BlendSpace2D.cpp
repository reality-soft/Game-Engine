#include "stdafx.h"
#include "BlendSpace2D.h"
#include "ResourceMgr.h"

namespace reality {
    AnimationClip::AnimationClip(float x, float y, bool is_valid, string animation_id) :
        x_(x), y_(y), is_valid_(is_valid), animation_id_(animation_id)
    {
        RESOURCE->UseResource<OutAnimData>(animation_id)->animation_matrices;
    }

    void BlendSpace2D::OnUpdate()
    {
        float clamped_x = max(x_min_, min(x_, x_max_));
        float clamped_y = max(y_min_, min(y_, y_max_));

        AnimationClip clips[4];

        clips[0] = GetClip(clamped_x, clamped_y, BLEND_DIRECTION::H_X_H_Y);
        clips[1] = GetClip(clamped_x, clamped_y, BLEND_DIRECTION::H_X_L_Y);
        clips[2] = GetClip(clamped_x, clamped_y, BLEND_DIRECTION::L_X_H_Y);
        clips[3] = GetClip(clamped_x, clamped_y, BLEND_DIRECTION::L_X_L_Y);

        float dists[4];
        float weights[4];
        float total_dist = 0.0f;
        for (int i = 0;i < 4;i++) {
            if (clips[i].is_valid_ == false) {
                continue;
            }
            dists[i] = (clamped_x - clips[i].x_) * (clamped_x - clips[i].x_) + (clamped_y - clips[i].y_) * (clamped_y - clips[i].y_);
            total_dist += dists[i];
        }

        for (int i = 0;i < 4;i++) {
            if (clips[i].is_valid_ == false) {
                weights[i] = 0.0f;
            }
            else {
                weights[i] = dists[i] / total_dist;
            }
        }

        unordered_map<string, unordered_map<UINT, XMMATRIX>> applied_animations;

        for (int i = 0;i < 4;i++) {
            if (applied_animations.find(clips[i].animation_id_) == applied_animations.end()) {
                OutAnimData* anim_resource = RESOURCE->UseResource<OutAnimData>(clips[i].animation_id_);
                if (anim_resource == nullptr) {
                    continue;
                }
                std::map<UINT, std::vector<XMMATRIX>>& animation_matrices = anim_resource->animation_matrices;

                anim_resource->animation_matrices;
                applied_animations.insert({ clips[i].animation_id_, {} });
                for (const auto& bone_id_weight_pair : animation_.bone_id_to_weight_) {
                    UINT bone_id = bone_id_weight_pair.first;
                    applied_animations[clips[i].animation_id_].insert({ bone_id, animation_matrices[bone_id][animation_.cur_frame_] });
                }
            }
        }

        for (const auto& bone_id_weight_pair : animation_.bone_id_to_weight_) {
            UINT bone_id = bone_id_weight_pair.first;

            XMMATRIX& anim_matrix1 = applied_animations[clips[0].animation_id_][bone_id];
            XMMATRIX& anim_matrix2 = applied_animations[clips[1].animation_id_][bone_id];
            XMMATRIX& anim_matrix3 = applied_animations[clips[2].animation_id_][bone_id];
            XMMATRIX& anim_matrix4 = applied_animations[clips[3].animation_id_][bone_id];

            animation_.animation_matrices[bone_id] = anim_matrix1 * weights[0] + anim_matrix2 * weights[1] + anim_matrix3 * weights[2] + anim_matrix4 * weights[3];
        }
    }

    AnimationClip BlendSpace2D::GetClip(int x, int y, BLEND_DIRECTION blend_direction)
    {
        float min_dist = FLT_MAX;
        AnimationClip closest_clip;

        for (const auto& clip : animation_clips_)
        {
            switch (blend_direction) {
            case BLEND_DIRECTION::H_X_H_Y:
                if (clip.x_ < x || clip.y_ < y)
                    continue;
                break;
            case BLEND_DIRECTION::H_X_L_Y:
                if (clip.x_ < x || clip.y_ > y)
                    continue;
                break;
            case BLEND_DIRECTION::L_X_H_Y:
                if (clip.x_ > x || clip.y_ < y)
                    continue;
                break;
            case BLEND_DIRECTION::L_X_L_Y:
                if (clip.x_ > x || clip.y_ > y)
                    continue;
                break;
            }

            float dist = (clip.x_ - x) * (clip.x_ - x) + (clip.y_ - y) * (clip.y_ - y);
            if (dist < min_dist)
            {
                min_dist = dist;
                closest_clip = clip;
            }
        }

        return closest_clip;
    }


    void BlendSpace2D::AddClip(float x, float y, string animation_id)
    {
        float x_range = x_max_ - x_min_;
        float y_range = y_max_ - y_min_;

        int row = static_cast<int>((y - y_min_) / y_range * BLENDSPACE2D_ROW);
        int col = static_cast<int>((x - x_min_) / x_range * BLENDSPACE2D_COL);

        row = max(0, min(row, BLENDSPACE2D_ROW - 1));
        col = max(0, min(col, BLENDSPACE2D_COL - 1));

        AnimationClip animation_clip(x, y, true, animation_id);
        animation_clips_.push_back(animation_clip);
    }
}