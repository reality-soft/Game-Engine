#include "stdafx.h"
#include "UI_Text.h"
#include "WriteMgr.h"

using namespace reality;

void UI_Text::InitText(string text, XMFLOAT2 pos, float size, XMFLOAT4 color)
{
	UIBase::Init();
	text_ = text;
	SetLocalRectByMin(pos, 100.0f, 100.0f);
	size_ = size;
	color_ = color;
}

void UI_Text::RenderThisUI()
{
	WRITER->Draw(text_, rect_transform_.world_rect.min, size_, color_);
}
