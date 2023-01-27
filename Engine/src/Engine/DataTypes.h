#pragma once
#include "common.h"

namespace KGCA41B
{
	struct Vertex
	{
		Vertex() = default;
		~Vertex() = default;

		XMFLOAT3   p; // 위치 12
		XMFLOAT3   n; // 노말 24
		XMFLOAT4   c; // 컬러 40
		XMFLOAT2   t; // 텍스처 48


	};

	struct SkinnedVertex : public Vertex
	{
		XMFLOAT4   i; // 인덱스 64
		XMFLOAT4   w; // 가중치 80

		SkinnedVertex operator +=(const Vertex& vertex)
		{
			p = vertex.p;
			n = vertex.n;
			c = vertex.c;
			t = vertex.t;

			return *this;
		}
	};
}