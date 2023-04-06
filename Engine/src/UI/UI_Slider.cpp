#include "stdafx.h"
#include "UI_Slider.h"
#include "InputMgr.h"

using namespace reality;


void UI_Slider::InitSlider(string background, string normal, string hover, string push, string select, string disable)
{
	UIBase::Init();

	render_data_.tex_id = background;
	slider_ = make_shared<UI_Button>();
	slider_->InitButton(normal, hover, push, select, disable);
	AddChildUI(slider_);
}

void UI_Slider::Update()
{
	UIBase::Update();

	UpdateSliderPos();
}

void UI_Slider::UpdateSliderPos()
{
	if (slider_->GetCurrentState() == E_UIState::UI_PUSH)
	{
		float x = (float)DINPUT->GetMousePosition().x - rect_transform_.world_rect.min.x;
		x = max(x, 0.0f);
		x = min(x, rect_transform_.world_rect.width);
		slider_->SetLocalRectByCenter({ x, rect_transform_.world_rect.height / 2.0f }, 
			slider_->rect_transform_.world_rect.width, slider_->rect_transform_.world_rect.height);

		value_ = 100.0f * x / rect_transform_.world_rect.min.x;
	}
}

float UI_Slider::GetValue()
{
	return value_;
}

void UI_Slider::SetSliderLocalRect(float width, float height)
{
	slider_->SetLocalRectByCenter({ rect_transform_.world_rect.width / 2.0f, rect_transform_.world_rect.height / 2.0f }, width, height);
}
