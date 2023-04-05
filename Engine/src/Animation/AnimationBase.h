#pragma once

namespace reality {
	class AnimationBase {
	public:
		virtual void OnUpdate() {};
		virtual string GetCurAnimationId() {
			return "";
		};
		virtual float GetCurFrame() {
			return 0.0f;
		}
	};
}
