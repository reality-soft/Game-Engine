#pragma once
#include "UIBase.h"

namespace reality
{
	class DLL_API UI_Button : public UIBase
	{
	private:
		map<E_UIState, string> button_texture_list;
		E_UIState last_state_;
		string hover_sound_;
		string push_sound_;
		string select_sound_;
	public:
		void InitButton(string normal, string hover, string push = "", string select = "", string disable = "");
		void SetButtonSound(string hover, string push, string select);
		virtual void Update() override;

	private:
		bool MouseOnButton(const Rect& button_rect, const POINT& mouse_point);
		void UpdateButtonState();
		void UpdateButtonTexture();
	};
}


