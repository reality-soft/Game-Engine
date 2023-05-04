#include "stdafx.h"
#include "UI_Button.h"
#include "Engine.h"
#include "InputMgr.h"
#include "FmodMgr.h"

using namespace reality;


void UI_Button::InitButton(string normal, string hover, string push, string select, string disable)
{
	UIBase::Init();
	button_texture_list[E_UIState::UI_NORMAL] = normal;
	button_texture_list[E_UIState::UI_HOVER] = hover;
	button_texture_list[E_UIState::UI_PUSH] = push;
	button_texture_list[E_UIState::UI_SELECT] = select;
	button_texture_list[E_UIState::UI_DISABLED] = disable;

	last_state_ = current_state_;
}

void reality::UI_Button::SetButtonSound(string hover, string push, string select)
{
	hover_sound_ = hover;
	push_sound_ = push;
	select_sound_ = select;
}

void UI_Button::Update()
{
	UIBase::Update();

	UpdateButtonState();

	UpdateButtonTexture();
}

bool UI_Button::MouseOnButton(const Rect& button_rect, const POINT& mouse_point)
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
	if (!onoff_)
		return;
	auto resolution = ENGINE->GetWindowResolution();
	if (MouseOnButton(rect_transform_[resolution].world_rect, DINPUT->GetMousePosition()))
	{
		if (DINPUT->GetMouseState(L_BUTTON) == KeyState::KEY_PUSH || DINPUT->GetMouseState(L_BUTTON) == KeyState::KEY_HOLD)
		{
			current_state_ = E_UIState::UI_PUSH;
			if(last_state_ != E_UIState::UI_PUSH && push_sound_ != "")
				FMOD_MGR->Play(push_sound_, SFX, false, 1.0f, {});

		}
		else if (DINPUT->GetMouseState(L_BUTTON) == KeyState::KEY_UP)
		{
			current_state_ = E_UIState::UI_SELECT;
			if (last_state_ != E_UIState::UI_SELECT && select_sound_ != "")
				FMOD_MGR->Play(select_sound_, SFX, false, 1.0f, {});
		}
		else
		{
			current_state_ = E_UIState::UI_HOVER;
			if (last_state_ != E_UIState::UI_HOVER && hover_sound_ != "")
				FMOD_MGR->Play(hover_sound_, SFX, false, 1.0f, {});
		}
	}
	else if (current_state_ == E_UIState::UI_PUSH && DINPUT->GetMouseState(L_BUTTON) == KeyState::KEY_HOLD)
	{
		current_state_ = E_UIState::UI_PUSH;
		if (last_state_ != E_UIState::UI_PUSH && select_sound_ != "")
			FMOD_MGR->Play(push_sound_, SFX, false, 1.0f, {});
	}
	else
	{
		current_state_ = E_UIState::UI_NORMAL;
	}

	last_state_ = current_state_;
	//OutputDebugString(to_wstring((int)last_state_).c_str());
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
