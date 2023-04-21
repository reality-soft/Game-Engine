#include "stdafx.h"
#include "BlendSpace2D.h"
#include "ResourceMgr.h"

namespace reality {
    AnimationClip::AnimationClip(float x, float y, string animation_id) : 
        x_(x), y_(y)
    {
        RESOURCE->UseResource<OutAnimData>(animation_id)->animation_matrices;
    }

    void BlendSpace2D::OnUpdate()
    {
        float clamped_x = max(x_min_, min(x_, x_max_));
        float clamped_y = max(y_min_, min(y_, y_max_));

        float x_range = x_max_ - x_min_;
        float y_range = y_max_ - y_min_;
        float x_pos = (clamped_x - x_min_) / x_range * (cols_ - 1);
        float y_pos = (clamped_y - y_min_) / y_range * (rows_ - 1);
        int x1 = static_cast<int>(std::floor(x_pos));
        int y1 = static_cast<int>(std::floor(y_pos));
        int x2 = static_cast<int>(std::ceil(x_pos));
        int y2 = static_cast<int>(std::ceil(y_pos));

        AnimationClip clip1 = GetClip(x1, y1);
        AnimationClip clip2 = GetClip(x2, y1);
        AnimationClip clip3 = GetClip(x1, y2);
        AnimationClip clip4 = GetClip(x2, y2);

        float dist1 = (clamped_x - clip1.x_) * (clamped_x - clip1.x_) + (clamped_y - clip1.y_) * (clamped_y - clip1.y_);
        float dist2 = (clamped_x - clip2.x_) * (clamped_x - clip2.x_) + (clamped_y - clip2.y_) * (clamped_y - clip2.y_);
        float dist3 = (clamped_x - clip3.x_) * (clamped_x - clip3.x_) + (clamped_y - clip3.y_) * (clamped_y - clip3.y_);
        float dist4 = (clamped_x - clip4.x_) * (clamped_x - clip4.x_) + (clamped_y - clip4.y_) * (clamped_y - clip4.y_);
        float total_dist = dist1 + dist2 + dist3 + dist4;
        float weight1 = total_dist > 0 ? (dist2 + dist4) / total_dist : 0.5f;
        float weight2 = total_dist > 0 ? (dist1 + dist3) / total_dist : 0.5f;
        float weight3 = total_dist > 0 ? (dist2 + dist4) / total_dist : 0.5f;
        float weight4 = total_dist > 0 ? (dist1 + dist3) / total_dist : 0.5f;

        string animation_ids[4];
        unordered_map<string, unordered_map<UINT, XMMATRIX>> applied_animations;

        animation_ids[0] = clip1.animation_id_;
        animation_ids[1] = clip2.animation_id_;
        animation_ids[2] = clip3.animation_id_;
        animation_ids[3] = clip4.animation_id_;

        for (int i = 0;i < 4;i++) {
            if (applied_animations.find(animation_ids[i]) == applied_animations.end()) {
                OutAnimData* anim_resource = RESOURCE->UseResource<OutAnimData>(animation_ids[i]);
                if (anim_resource == nullptr) {
                    continue;
                }
                std::map<UINT, std::vector<XMMATRIX>>& animation_matrices = anim_resource->animation_matrices;

                anim_resource->animation_matrices;
                applied_animations.insert({ animation_ids[i], {} });
                for (const auto& bone_id_weight_pair : animation_.bone_id_to_weight_) {
                    UINT bone_id = bone_id_weight_pair.first;
                    applied_animations[animation_ids[i]].insert({ bone_id, animation_matrices[bone_id][animation_.cur_frame_] });
                }
            }
        }

        for (const auto& bone_id_weight_pair : animation_.bone_id_to_weight_) {
            UINT bone_id = bone_id_weight_pair.first;
            
            XMMATRIX& anim_matrix1 = applied_animations[animation_ids[0]][bone_id];
            XMMATRIX& anim_matrix2 = applied_animations[animation_ids[0]][bone_id];
            XMMATRIX& anim_matrix3 = applied_animations[animation_ids[0]][bone_id];
            XMMATRIX& anim_matrix4 = applied_animations[animation_ids[0]][bone_id];

            animation_.animation_matrices[bone_id] = anim_matrix1 * weight1 + anim_matrix2 * weight2 + anim_matrix3 * weight3 + anim_matrix4 * weight4;
        }
    }

    void BlendSpace2D::AddClip(float x, float y, string animation_id)
    {
        float x_range = x_max_ - x_min_;
        float y_range = y_max_ - y_min_;

        int row = static_cast<int>((y - y_min_) / y_range * BLENDSPACE2D_ROW);
        int col = static_cast<int>((x - x_min_) / x_range * BLENDSPACE2D_COL);

        row = max(0, min(row, BLENDSPACE2D_ROW - 1));
        col = max(0, min(col, BLENDSPACE2D_COL - 1));

        animation_clips_[row * BLENDSPACE2D_COL + col] = AnimationClip(x, y, animation_id);
    }
}