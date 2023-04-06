#pragma once
#include "UI.h"

namespace reality
{
	class DLL_API UIBase
	{
	protected:
		bool						onoff_;
		E_UIState					current_state_;
	public:
		UIBase*						parent_ui_;
		vector<shared_ptr<UIBase>>	child_ui_list_;
	public:
		RectTransform	rect_transform_;
		RectRenderData	render_data_;
	protected:
		virtual void Init();
	public:
		virtual void Update();
		virtual void Render();
	private:
		void CreateRenderData();
	protected:
		virtual void UpdateThisUI();
		virtual void RenderThisUI();
		void UpdateRectTransform();
	public:
		void AddChildUI(shared_ptr<UIBase> child_ui);
		E_UIState GetCurrentState();
		void SetCurrentState(E_UIState state);
	public:
		bool GetOnOff();
		void On();
		void Off();
	public:
		void SetLocalRectByMin(XMFLOAT2 min, float width, float height);
		void SetLocalRectByMax(XMFLOAT2 max, float width, float height);
		void SetLocalRectByCenter(XMFLOAT2 center, float width, float height);
		void SetLocalRectByMinMax(XMFLOAT2 min, XMFLOAT2 max);
	};
}

