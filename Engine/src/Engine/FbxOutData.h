#pragma once
#pragma warning(disable: 4099)
#include "DataTypes.h"
#include <fbxsdk.h>

using namespace std;

namespace KGCA41B
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

		string mesh_name;
		string material_name;
		vector<Vertex> vertices;
		vector<SkinnedVertex> skinned_vertices;
		vector<UINT>   indices;
		vector<IndexWeight> index_weight;
		map<UINT, XMMATRIX> bind_poses;
	};

	struct OutAnimData
	{
		map<UINT, vector<XMMATRIX>> animations;
		UINT start_frame = 0;
		UINT end_frame = 0;
	};
}

