#include "stdafx.h"
#include "UI_Slider.h"
#include "Engine.h"
#include "InputMgr.h"

using namespace reality;


void UI_Slider::InitSlider(string background, string normal, string hover, string push, string select, string disable)
{
	UIBase::Init();

	render_data_.tex_id = background;
	slider_ = make_shared<UI_Button>();
	slider_->InitButton(normal, hover, push, select, disable);
	AddChildUI("1_Slider", slider_);
	value_ = 50.0f;
	last_value_ = 50.0f;
}

void UI_Slider::Update()
{
	UIBase::Update();

	UpdateSliderPos();

	if (value_ != last_value_)
		is_changed_ = true;
	else
		is_changed_ = false;

	last_value_ = value_;
}

void UI_Slider::UpdateSliderPos()
{
	if (slider_->GetCurrentState() == E_UIState::UI_PUSH)
	{
		auto resolution = ENGINE->GetWindowResolution();
		float x = (float)DINPUT->GetMousePosition().x - rect_transform_[resolution].world_rect.min.x;
		x = max(x, 0.0f);
		x = min(x, rect_transform_[resolution].world_rect.width);
		slider_->SetLocalRectByCenter({ x * 1.0f / E_Resolution_Multiply[resolution], rect_transform_[E_Resolution::R1920x1080].world_rect.height / 2.0f},
			slider_->rect_transform_[E_Resolution::R1920x1080].world_rect.width, slider_->rect_transform_[E_Resolution::R1920x1080].world_rect.height);

		value_ = 100.0f * x / rect_transform_[resolution].world_rect.width;
	}
}

float UI_Slider::GetValue()
{
	return value_;
}

void UI_Slider::SetSliderLocalRect(float width, float height)
{
	slider_->SetLocalRectByCenter({ rect_transform_[E_Resolution::R1920x1080].world_rect.width / 2.0f, rect_transform_[E_Resolution::R1920x1080].world_rect.height / 2.0f }, width, height);
}
