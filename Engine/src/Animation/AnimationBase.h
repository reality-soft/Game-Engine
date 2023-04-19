#pragma once
#include "Animation.h"

namespace reality {
	enum class ANIM_STATE {
		ANIM_STATE_NONE,
		ANIM_STATE_CUR_ONLY,
		ANIM_STATE_PREV_ONLY,
		ANIM_STATE_CUR_PREV,
	};

	class DLL_API AnimationBase {
	public:
		AnimationBase(string skeletal_mesh_id, string bone_name, int range, int num_of_bones);

	public:
		Animation GetCurAnimation() {
			return animation_;
		}

	public:
		virtual void AnimationUpdate();
		virtual void OnInit() {};

	protected:
		virtual void OnUpdate() {};

	public:
		bool IsAnimationEnded() {
			return animation_ended_;
		}

	public:
		virtual string GetCurAnimationId();
		virtual float GetCurAnimTime();
		virtual float GetCurFrame();
		virtual float GetBlendTime();

	public:
		ANIM_STATE GetCurAnimState();

	public:
		virtual void SetAnimation(string animation_id, float blend_time);

	protected:
		Animation animation_;
		ANIM_STATE cur_anim_state_ = ANIM_STATE::ANIM_STATE_NONE;
		bool animation_ended_ = false;
	};
}

