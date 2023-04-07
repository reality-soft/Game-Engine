#pragma once
#include "stdafx.h"

namespace reality {
	struct DLL_API Animation {
		string prev_anim_id_ = "";
		string cur_anim_id_ = "";
		float blend_time_ = 0.0f;
		float cur_animation_time_ = 0.0f;
		float prev_anim_last_frame_ = 0.0f;
		float cur_frame_ = 0.0f;
	};
}