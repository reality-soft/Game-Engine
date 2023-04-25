#pragma once
#include "UI.h"

namespace reality
{
	enum E_Resolution;

	class DLL_API UIBase
	{
	protected:
		bool		onoff_;
		E_UIState	current_state_;
	public:
		UIBase*								parent_ui_;
		map<string, shared_ptr<UIBase>>		child_ui_list_;
	public:
		vector<RectTransform> rect_transform_;
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
	public:
		void AddChildUI(string name, shared_ptr<UIBase> child_ui);
		void DeleteChildUI(string name);
		E_UIState GetCurrentState();
		void SetCurrentState(E_UIState state);
		void UpdateRectTransform();
	public:
		bool GetOnOff();
		void On();
		void Off();
		void SetAlpha(float alpha);
	public:
		virtual void SetLocalRectByMin(XMFLOAT2 min, float width, float height);
		virtual void SetLocalRectByMax(XMFLOAT2 max, float width, float height);
		virtual void SetLocalRectByCenter(XMFLOAT2 center, float width, float height);
		virtual void SetLocalRectByMinMax(XMFLOAT2 min, XMFLOAT2 max);
	private:
		virtual void ComputeLocalRectByMin(E_Resolution resolution, XMFLOAT2 min, float width, float height);
		virtual void ComputeLocalRectByMax(E_Resolution resolution, XMFLOAT2 max, float width, float height);
		virtual void ComputeLocalRectByCenter(E_Resolution resolution, XMFLOAT2 center, float width, float height);
		virtual void ComputeLocalRectByMinMax(E_Resolution resolution, XMFLOAT2 min, XMFLOAT2 max);
	};
}

