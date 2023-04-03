#include "stdafx.h"
#include "UI_Text.h"

using namespace reality;

void UI_Text::InitText(string text, E_Font font, XMFLOAT2 pos, float size, XMFLOAT4 color)
{
	UIBase::Init();
	text_ = text;
	font_ = font;
	SetLocalRectByMin(pos, 100.0f, 100.0f);
	size_ = size;
	color_ = color;
}

void UI_Text::SetText(string text)
{
	text_ = text;
}

void UI_Text::SetFont(E_Font font)
{
	font_ = font;
}

void UI_Text::RenderThisUI()
{
	WRITER->Draw(text_, font_, rect_transform_.world_rect.min, size_, color_);
}
