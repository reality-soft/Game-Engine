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
		map<UINT, Bone> id_bone_map;
		map<string, UINT> bone_name_id_map;

		unordered_set<UINT> GetSubBonesOf(string bone_name) {
			unordered_set<UINT> sub_bones;
			queue<UINT> cur_bone_ids;

			sub_bones.insert(bone_name_id_map[bone_name]);
			cur_bone_ids.push(bone_name_id_map[bone_name]);
			while (true) {
				UINT cur_bone_id = cur_bone_ids.front();
				cur_bone_ids.pop();

				for (auto child_bone_id : id_bone_map[cur_bone_id].child_bone_ids) {
					sub_bones.insert(child_bone_id);
					cur_bone_ids.push(child_bone_id);
				}

				if (cur_bone_ids.empty()) {
					break;
				}
			}
		}
	};
}