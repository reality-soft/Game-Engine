#pragma once
#include "stdafx.h"

namespace reality {
	struct DLL_API Animation {
		Animation(int size) {
			animation_matrices.resize(size);
		}
		vector<XMMATRIX> animation_matrices;
		vector<XMMATRIX> prev_animation_matrices;
		string cur_anim_id_ = "";
		float start_frame_ = 0.0f;
		float end_frame_ = 0.0f;
		float blend_time_ = 0.0f;
		float cur_animation_time_ = 0.0f;
		float cur_frame_ = 0.0f;
	};
}