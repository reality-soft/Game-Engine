#include "stdafx.h"
#include "UI_Button.h"
#include "InputMgr.h"

using namespace reality;


void UI_Button::InitButton(string normal, string hover, string push, string select, string disable)
{
	UIBase::Init();
	button_texture_list.insert({ E_UIState::UI_NORMAL, normal });
	button_texture_list.insert({ E_UIState::UI_HOVER, hover });
	button_texture_list.insert({ E_UIState::UI_PUSH, push });
	button_texture_list.insert({ E_UIState::UI_SELECT, select });
	button_texture_list.insert({ E_UIState::UI_DISABLED, disable });
}

void UI_Button::Update()
{
	UIBase::Update();

	UpdateButtonState();

	UpdateButtonTexture();
}

bool UI_Button::MouseOnButton(Rect& button_rect, POINT& mouse_point)
{
	if ((button_rect.min.x <= mouse_point.x && mouse_point.x <= button_rect.max.x))
	{
		if ((button_rect.min.y <= mouse_point.y && mouse_point.y <= button_rect.max.y))
		{
			return true;
		}
	}
	return false;
}

void UI_Button::UpdateButtonState()
{
	if (MouseOnButton(rect_transform_.world_rect, DINPUT->GetMousePosition()))
	{
		current_state_ = E_UIState::UI_HOVER;
		if (DINPUT->GetMouseState(L_BUTTON) == KeyState::KEY_PUSH || DINPUT->GetMouseState(L_BUTTON) == KeyState::KEY_HOLD)
		{
			current_state_ = E_UIState::UI_PUSH;
		}
		if (DINPUT->GetMouseState(L_BUTTON) == KeyState::KEY_UP)
		{
			current_state_ = E_UIState::UI_SELECT;
		}
	}
	else
	{
		current_state_ = E_UIState::UI_NORMAL;
	}
}

void UI_Button::UpdateButtonTexture()
{
	if (button_texture_list[current_state_] == "")
	{
		render_data_.tex_id = button_texture_list[E_UIState::UI_NORMAL];
	}
	else
	{
		render_data_.tex_id = button_texture_list[current_state_];
	}
	
}
