#pragma once
#include "stdafx.h"

namespace reality {
	struct AnimSlot {
		map<int, unordered_set<UINT>> included_skeletons;
		unordered_map<UINT, int>	  bone_id_to_weight;
		string		 anim_id;
		float		 cur_frame = 0.0f;
		float		 range;
	};
}
