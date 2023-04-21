#pragma once
#include "stdafx.h"

namespace reality {
	struct Bone {
		UINT bone_id;
		UINT parent_bone_id = -1;
		vector<UINT> child_bone_ids;
	};

	struct Skeleton
	{
		Skeleton() = default;
		Skeleton(const Skeleton& other);

		map<UINT, XMMATRIX> bind_pose_matrices;
		map<UINT, Bone> id_bone_map;
		map<string, UINT> bone_name_id_map;

		unordered_map<UINT, int> GetSubBonesOf(string bone_name, int range) {
			unordered_map<UINT, int> out_id_weight_map;
			
			queue<pair<int, UINT>> cur_bone_ids;

			UINT center_bone_id = bone_name_id_map[bone_name];

			cur_bone_ids.push({ 0, center_bone_id });
			while (true) {
				if (cur_bone_ids.empty()) {
					break;
				}

				int cur_bone_depth = cur_bone_ids.front().first;
				UINT cur_bone_id = cur_bone_ids.front().second;
				cur_bone_ids.pop();

				if (id_bone_map.find(cur_bone_id) == id_bone_map.end()) {
					continue;
				}
				for (const auto& child_bone_id : id_bone_map[cur_bone_id].child_bone_ids) {
					int child_bone_depth = cur_bone_depth + 1;
					cur_bone_ids.push({ child_bone_depth, child_bone_id });

					child_bone_depth = min(child_bone_depth, range);
					out_id_weight_map.insert({ child_bone_id, child_bone_depth + range });
				}
			}

			UINT cur_bone_id = id_bone_map[center_bone_id].parent_bone_id;
			for (int i = 1;i < range;i++) {
				if (cur_bone_id == -1) {
					break;
				}
				out_id_weight_map.insert({ cur_bone_id, 0 - i + range });
				cur_bone_id = id_bone_map[cur_bone_id].parent_bone_id;
			}

			return out_id_weight_map;
		}
	};
}