#pragma once
#include "DllMacro.h"
#include "DataTypes.h"

namespace reality
{
	

	class DLL_API UIBase
	{
	private:
		E_UIState					current_state_;
		shared_ptr<UIBase>			parent_ui_;
		vector<shared_ptr<UIBase>>	child_ui_list_;
	public:
		Transform2D		ui_transform_;
		Rect			ui_rect_;
		RectRenderData	render_data_;
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

