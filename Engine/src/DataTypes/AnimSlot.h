#pragma once
#include "stdafx.h"

namespace reality {
	struct AnimSlot {
		unordered_set<UINT> included_skeletons;
		string		 anim_id;
		float		 cur_frame = 0.0f;
	};
}