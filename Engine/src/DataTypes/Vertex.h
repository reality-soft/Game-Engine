#pragma once
#include "stdafx.h"

namespace reality
{
	struct Vertex
	{
		XMFLOAT3   p;
		XMFLOAT3   n;
		XMFLOAT4   c;
		XMFLOAT2   t;
	};

	struct LightVertex
	{
		XMFLOAT3 p;
		XMFLOAT2 t;
	};

	struct LevelVertex
	{
		XMFLOAT3   p;
		XMFLOAT3   n;
		XMFLOAT4   c;
		XMFLOAT2   t;
		XMFLOAT2   t_layer;
	};

	struct SkinnedVertex
	{
		XMFLOAT3   p;
		XMFLOAT3   n;
		XMFLOAT4   c;
		XMFLOAT2   t;
		XMFLOAT4   i;
		XMFLOAT4   w;

		SkinnedVertex operator =(const Vertex& vertex)
		{
			this->p = vertex.p;
			this->n = vertex.n;
			this->c = vertex.c;
			this->t = vertex.t;

			return *this;
		}
	};

	struct EffectVertex
	{
		XMFLOAT3   p;
		XMFLOAT4   c;
		XMFLOAT2   t;
	};

	struct UIVertex
	{
		XMFLOAT2 p;
		XMFLOAT2 t;
	};
}