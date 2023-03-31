#pragma once
#include "stdafx.h"

namespace reality {
	struct Bone {
		UINT bone_id;
		vector<UINT> child_bone_ids;
	};

	struct Skeleton
	{
		Skeleton() = default;
		Skeleton(const Skeleton& other);

		map<UINT, XMMATRIX> bind_pose_matrices;
		map<string, Bone> skeleton_id_map;
	};
}