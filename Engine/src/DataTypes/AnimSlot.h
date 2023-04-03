#pragma once
#include "stdafx.h"

namespace reality {
	struct AnimSlot {
		map<int, unordered_set<UINT>> included_skeletons;
		string		 anim_id;
		float		 cur_frame = 0.0f;
		float		 range;
	};
}
