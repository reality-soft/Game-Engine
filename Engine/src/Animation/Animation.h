#pragma once
#include "stdafx.h"
#include "AnimNotify.h"

namespace reality {
	struct DLL_API Animation {
		unordered_map<UINT, XMMATRIX> animation_matrices;
		unordered_map<UINT, XMMATRIX> prev_animation_matrices;
		unordered_map<UINT, int>	  bone_id_to_weight_;
		float range_;
		string cur_anim_id_ = "";
		float start_frame_ = 0.0f;
		float end_frame_ = 0.0f;
		float blend_time_ = 0.0f;
		float cur_animation_time_ = 0.0f;
		float cur_frame_ = 0.0f;
		vector<AnimNotify> notifies_;
		bool loop_;
	};
}