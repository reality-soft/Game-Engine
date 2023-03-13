#pragma once
#include "DllMacro.h"
#include "DataTypes.h"

namespace reality
{
	enum class E_UIState
	{
		UI_NORMAL = 0,
		UI_HOVER = 1,
		UI_PUSH = 2,
		UI_SELECT = 3,
		UI_DISABLED = 4,
	};

	struct UIRect
	{
		XMFLOAT2	min;
		XMFLOAT2	max;
		XMFLOAT2	center;
		float		width;
		float		height;
	};

	struct Transform2D
	{
		XMMATRIX local;
		XMMATRIX world;
	};

	struct UIRenderData
	{
		string vs_id;
		string ps_id;
		string tex_id;
		vector<UIVertex>		vertex_list;
		ComPtr<ID3D11Buffer>	vertex_buffer;
		vector<DWORD>			index_list;
		ComPtr<ID3D11Buffer>	index_buffer;
	};

	class DLL_API UIBase
	{
	private:
		E_UIState					current_state_;
		shared_ptr<UIBase>			parent_ui_;
		vector<shared_ptr<UIBase>>	child_ui_list_;
	protected:
		Transform2D		ui_transform_;
		UIRect			ui_rect_;
		UIRenderData	render_data_;
	protected:
		virtual void Init();
	public:
		virtual void Update();
		virtual void Render();
	private:
		void CreateRenderData();
	private:
		virtual void UpdateThisUI();
		virtual void RenderThisUI();
	public:
		void SetRectByMin(XMFLOAT2 min, float width, float height);
		void SetRectByMax(XMFLOAT2 max, float width, float height);
		void SetRectByCenter(XMFLOAT2 center, float width, float height);
		void SetRectByMinMax(XMFLOAT2 min, XMFLOAT2 max);
	};
}

