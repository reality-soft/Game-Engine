#pragma once
#include "Vertex.h"

namespace reality
{
	struct AnimTrack
	{
		UINT frame;
		XMMATRIX anim_mat;
		XMVECTOR translation_vec;
		XMVECTOR rotation_vec;
		XMVECTOR scaling_vec;
	};

	struct AnimFrame
	{
		ULONG64 start;
		ULONG64 end;
	};

	struct IndexWeight
	{
		int index[8] = { 0, };
		float weight[8] = { 0, };

		void insert(int index_, float weight_);
	};

	struct OutMeshData
	{
		bool is_skinned = false;

		std::string mesh_name;
		std::string material_name;
		std::vector<Vertex> vertices;
		std::vector<LightVertex> light_vertices;
		std::vector<SkinnedVertex> skinned_vertices_by_control_point;
		std::vector<SkinnedVertex> skinned_vertices_by_polygon_vertex;
		std::vector<UINT>   indices;
		std::vector<IndexWeight> index_weight;
		std::map<UINT, XMMATRIX> bind_poses;
		std::map<string, UINT> skelton_id_map;
	};

	struct OutAnimData
	{
		std::map<UINT, std::vector<XMMATRIX>> animations;
		UINT start_frame = 0;
		UINT end_frame = 0;
	};
}

