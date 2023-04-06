#pragma once
#include "Animation.h"
#include "TimeMgr.h"

namespace reality {
	class AnimationBase {
	protected:
		Animation animation_;
	
	public:
		Animation GetCurAnimation() {
			return animation_;
		}

	protected:
		virtual void OnUpdate() {
		};

	public:
		virtual void AnimationUpdate() {
			
			OnUpdate();
		}
		virtual string GetCurAnimationId() {
			return "";
		};
		virtual float GetCurFrame() {
			return 0.0f;
		}
	};
}

