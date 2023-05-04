#include "stdafx.h"
#include "AnimationBase.h"
#include "TimeMgr.h"
#include "ResourceMgr.h"
#include "EventMgr.h"

reality::AnimationBase::AnimationBase(string skeletal_mesh_id, int range, string bone_name)
{
	SkeletalMesh* skeletal_mesh = RESOURCE->UseResource<SkeletalMesh>(skeletal_mesh_id);
	// base slot
	if (bone_name == "") {
		const auto& id_bone_map = skeletal_mesh->skeleton.id_bone_map;

		for (const auto& id_bone_pair : id_bone_map) {
			const int& bone_id = id_bone_pair.first;
			animation_.bone_id_to_weight_.insert({ bone_id , 1.0f });
		}
	}
	else {
		animation_.bone_id_to_weight_ = skeletal_mesh->skeleton.GetSubBonesOf(bone_name, range);
		animation_.range_ = range * 2;
	}
}

void reality::AnimationBase::AnimationUpdate()
{
	animation_.cur_animation_time_ += TM_DELTATIME;

	animation_.cur_frame_ += 60.0f / TM_FPS;

	if (animation_.cur_frame_ >= animation_.end_frame_) {
		animation_ended_ = true;
		animation_.cur_frame_ = animation_.start_frame_;

		if (animation_.loop_ == false) {
			SetAnimation("", 0.2f, true);
		}

		for (auto& cur_notify : animation_.notifies_) {
			cur_notify.is_managed = false;
		}
	}

	for (const auto& cur_notify : animation_.notifies_) {
		if (cur_notify.is_managed == true) {
			continue;
		}

		if (cur_notify.frame <= animation_.cur_frame_) {
			EVENT->PushEvent(cur_notify.event);
		}
	}

	OnUpdate();

	animation_ended_ = false;

	OutAnimData* cur_animation_resource = RESOURCE->UseResource<OutAnimData>(animation_.cur_anim_id_);
	if (cur_animation_resource == nullptr) {
		return;
	}

	for (const auto& bone_id_weight_pair : animation_.bone_id_to_weight_) {
		const UINT& bone_id = bone_id_weight_pair.first;
		animation_.animation_matrices[bone_id] = cur_animation_resource->animation_matrices[bone_id][animation_.cur_frame_];
	}
}
string reality::AnimationBase::GetCurAnimationId()
{
	return animation_.cur_anim_id_;
}
float reality::AnimationBase::GetCurAnimTime()
{
	return animation_.cur_animation_time_;
}

float reality::AnimationBase::GetCurFrame()
{
	return animation_.cur_frame_;
}

float reality::AnimationBase::GetBlendTime()
{
	return animation_.blend_time_;
}

float reality::AnimationBase::GetRange()
{
	return animation_.range_;
}

unordered_map<UINT, XMMATRIX>* reality::AnimationBase::GetAnimationMatrices()
{
	return &animation_.animation_matrices;
}

unordered_map<UINT, XMMATRIX>* reality::AnimationBase::GetPrevAnimationMatrices()
{
	return &animation_.prev_animation_matrices;
}

unordered_map<UINT, int>* reality::AnimationBase::GetWeights()
{
	return &animation_.bone_id_to_weight_;
}

reality::ANIM_STATE reality::AnimationBase::GetCurAnimState()
{
	return cur_anim_state_;
}

void reality::AnimationBase::SetAnimation(string animation_id, float blend_time, bool loop, vector<AnimNotify> notifies)
{
	OutAnimData* anim_resource = RESOURCE->UseResource<OutAnimData>(animation_id);
	OutAnimData* prev_anim_resource = RESOURCE->UseResource<OutAnimData>(animation_.cur_anim_id_);

	string prev_animation_id = animation_.cur_anim_id_;
	float prev_anim_last_frame = animation_.cur_frame_;

	if (anim_resource != nullptr) {
		animation_.start_frame_ = anim_resource->start_frame;
		animation_.end_frame_ = anim_resource->end_frame;
		animation_.cur_frame_ = anim_resource->start_frame;
	}

	if (prev_anim_resource != nullptr) {
		for (const auto& bone_id_weight_pair : animation_.bone_id_to_weight_) {
			const UINT& bone_id = bone_id_weight_pair.first;
			animation_.prev_animation_matrices[bone_id] = prev_anim_resource->animation_matrices[bone_id][prev_anim_last_frame];
		}
	}

	animation_.cur_anim_id_ = animation_id;
	animation_.cur_animation_time_ = 0.0f;
	animation_.blend_time_ = blend_time;

	if (prev_anim_resource == nullptr && anim_resource == nullptr) {
		cur_anim_state_ = ANIM_STATE::ANIM_STATE_NONE;
	}
	else if (prev_anim_resource == nullptr) {
		cur_anim_state_ = ANIM_STATE::ANIM_STATE_CUR_ONLY;
	}
	else if (anim_resource == nullptr) {
		cur_anim_state_ = ANIM_STATE::ANIM_STATE_PREV_ONLY;
	}
	else {
		cur_anim_state_ = ANIM_STATE::ANIM_STATE_CUR_PREV;
	}
	
	animation_.notifies_ = notifies;

	animation_.loop_ = loop;
}
