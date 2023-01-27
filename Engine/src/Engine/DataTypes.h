#pragma once
#include "common.h"
#include <d3d11.h>

namespace KGCA41B
{
	struct Vertex
	{
		XMFLOAT3   p;
		XMFLOAT3   n;
		XMFLOAT4   c;
		XMFLOAT2   t;
	};

	struct SkinnedVertex : public Vertex
	{
		XMFLOAT4   i;
		XMFLOAT4   w;

		SkinnedVertex operator +=(const Vertex& vertex)
		{
			p = vertex.p;
			n = vertex.n;
			c = vertex.c;
			t = vertex.t;

			return *this;
		}
	};

	struct SingleMesh
	{
		vector<Vertex> vertices;
		ID3D11Buffer* vertex_buffer;
	};

	struct CbTransform
	{
		XMMATRIX world_matrix;
		XMMATRIX view_matrix;
		XMMATRIX projection_matrix;
	};

	struct CbSkeleton
	{
		XMMATRIX  mat_skeleton[255];
	};

	enum class ActionType
	{
		JUMP,
		SIT,
		DASH,

		ATTACK,
		THROW,

		SKILL1,
		SKILL2,
		SKILL3,

		IDLE,
	};
}