#include "stdafx.h"
#include "AnimationBase.h"
#include "TimeMgr.h"
#include "ResourceMgr.h"

void reality::AnimationBase::AnimationUpdate()
{
	OnUpdate();

	animation_.cur_animation_time_ += TM_DELTATIME;

	if (cur_anim_state_ == ANIM_STATE::ANIM_STATE_NONE||
		cur_anim_state_ == ANIM_STATE::ANIM_STATE_PREV_ONLY) {
		return;
	}
	animation_.cur_frame_ += 60.0f / TM_FPS;

	OutAnimData* anim_resource = RESOURCE->UseResource<OutAnimData>(animation_.cur_anim_id_);
	if (animation_.cur_frame_ >= anim_resource->end_frame) {
		animation_ended_ = true;
		animation_.cur_frame_ = anim_resource->start_frame;
	}

	OnUpdate();

	animation_ended_ = false;
}
string reality::AnimationBase::GetCurAnimationId()
{
	return animation_.cur_anim_id_;
}
string reality::AnimationBase::GetPrevAnimationId()
{
	return animation_.prev_anim_id_;
}
float reality::AnimationBase::GetPrevAnimLastFrame()
{
	return animation_.prev_anim_last_frame_;
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

reality::ANIM_STATE reality::AnimationBase::GetCurAnimState()
{
	return cur_anim_state_;
}

void reality::AnimationBase::SetAnimation(string animation_id, float blend_time)
{
	OutAnimData* anim_resource = RESOURCE->UseResource<OutAnimData>(animation_id);
	OutAnimData* prev_anim_resource = RESOURCE->UseResource<OutAnimData>(animation_.cur_anim_id_);


	string prev_animation_id = animation_.cur_anim_id_;
	float prev_anim_last_frame = animation_.cur_frame_;


	animation_.cur_anim_id_ = animation_id;
	if (anim_resource != nullptr) {
		animation_.cur_frame_ = anim_resource->start_frame;
	}
	animation_.cur_animation_time_ = 0.0f;

	animation_.prev_anim_id_ = prev_animation_id;
	animation_.prev_anim_last_frame_ = prev_anim_last_frame;
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
}
