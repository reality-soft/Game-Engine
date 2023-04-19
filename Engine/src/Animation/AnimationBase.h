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
		AnimationBase(int num_of_bones);

	public:
		Animation* GetCurAnimation() {
			return animation_.get();
		}

	public:
		virtual void OnInit() {};

	protected:
		virtual void OnUpdate() {};

	public:
		bool IsAnimationEnded() {
			return animation_ended_;
		}

	public:
		virtual void AnimationUpdate();
		virtual string GetCurAnimationId();
		virtual string GetPrevAnimationId();
		virtual float GetPrevAnimLastFrame();
		virtual float GetCurAnimTime();
		virtual float GetCurFrame();
		virtual float GetBlendTime();

	public:
		ANIM_STATE GetCurAnimState();

	public:
		virtual void SetAnimation(string animation_id, float blend_time);

	protected:
		shared_ptr<Animation> animation_;
		ANIM_STATE cur_anim_state_ = ANIM_STATE::ANIM_STATE_NONE;
		bool animation_ended_ = false;
	};
}

