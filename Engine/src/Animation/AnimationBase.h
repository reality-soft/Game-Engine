#pragma once
#include "Animation.h"

namespace reality {
	class DLL_API AnimationBase {
	protected:
		Animation animation_;
		bool is_valid_ = false;

	public:
		Animation GetCurAnimation() {
			return animation_;
		}

	protected:
		virtual void OnUpdate() {};

	public:
		virtual void AnimationUpdate();
		virtual string GetCurAnimationId();
		virtual string GetPrevAnimationId();
		virtual float GetPrevAnimLastFrame();
		virtual float GetCurAnimTime();
		virtual float GetCurFrame();
		virtual float GetBlendTime();

	public:
		bool IsValid();

	public:
		virtual void SetAnimation(string animation_id, float blend_time);
	};
}

