#pragma once
#include "UIBase.h"

namespace reality
{
	class DLL_API UI_Button : public UIBase
	{
	private:
		map<E_UIState, string> button_texture_list;
	public:
		void InitButton(string normal, string hover, string push = "", string select = "", string disable = "");
		virtual void Update() override;

	private:
		bool MouseOnButton(Rect& button_rect, POINT& mouse_point);
		void UpdateButtonState();
		void UpdateButtonTexture();
	};
}


