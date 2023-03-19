#pragma once
#include "stdafx.h"
#include "Vertex.h"

namespace reality
{
	// UI
	enum class E_UIState
	{
		UI_NORMAL = 0,
		UI_HOVER = 1,
		UI_PUSH = 2,
		UI_SELECT = 3,
		UI_DISABLED = 4,
	};

	struct Rect
	{
		XMFLOAT2	min;
		XMFLOAT2	max;
		XMFLOAT2	center;
		float		width;
		float		height;
		void SetRectByMin(XMFLOAT2 min, float width, float height)
		{
			this->min = min;
			this->width = width;
			this->height = height;
			this->max = { min.x + width, min.y + height };
			this->center = { min.x + width / 2.0f, min.y + height / 2.0f };
		}

		void SetRectByMax(XMFLOAT2 max, float width, float height)
		{
			this->max = max;
			this->width = width;
			this->height = height;
			this->min = { max.x - width, max.y - height };
			this->center = { max.x - width / 2.0f, max.y - height / 2.0f };
		}

		void SetRectByCenter(XMFLOAT2 center, float width, float height)
		{
			this->center = center;
			this->width = width;
			this->height = height;
			this->min = { center.x - width / 2.0f, center.y - width / 2.0f };
			this->max = { center.x + width / 2.0f, center.y + width / 2.0f };
		}

		void SetRectByMinMax(XMFLOAT2 min, XMFLOAT2 max)
		{
			this->min = min;
			this->max = max;
			this->width = max.x - min.x;
			this->height = max.y - min.y;
			this->center = { min.x + (this->width / 2.0f), min.y + (this->height / 2.0f) };
		}
	};

	struct CbUI
	{
		__declspec(align(16)) struct Data
		{
			XMMATRIX world;
		} data;
		ComPtr<ID3D11Buffer> buffer;
	};

	struct RectTransform
	{
		Rect world_rect;
		Rect local_rect;
		XMMATRIX local_matrix;
		XMMATRIX world_matrix;
	};

	struct RectRenderData
	{
		string vs_id;
		string ps_id;
		string tex_id;
		vector<UIVertex>		vertex_list;
		ComPtr<ID3D11Buffer>	vertex_buffer;
		vector<DWORD>			index_list;
		ComPtr<ID3D11Buffer>	index_buffer;
	};
}