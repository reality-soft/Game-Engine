#include "stdafx.h"
#include "AnimationBase.h"
#include "TimeMgr.h"
#include "ResourceMgr.h"

void reality::AnimationBase::AnimationUpdate()
{
	animation_.cur_frame_ = TM_DELTATIME;

	OnUpdate();

	OutAnimData* anim_resource = RESOURCE->UseResource<OutAnimData>(animation_.cur_anim_id_);
	if (animation_.cur_frame_ >= anim_resource->end_frame) {
		animation_.cur_frame_ = anim_resource->start_frame;
		animation_.blend_time_ = 0;
	}
}
string reality::AnimationBase::GetCurAnimationId()
{
	return "";
};

float reality::AnimationBase::GetCurFrame()
{
	return 0.0f;
}
