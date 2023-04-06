#pragma once
#include "Animation.h"

namespace reality {
	class AnimationBase {
	protected:
		Animation animation_;
	
	public:
		Animation GetCurAnimation() {
			return animation_;
		}

	protected:
		virtual void OnUpdate() {};

	public:
		virtual void AnimationUpdate();
		virtual string GetCurAnimationId();
		virtual float GetCurFrame();
	};
}

