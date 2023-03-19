#include "stdafx.h"
#include "UI_Image.h"

using namespace reality;

void UI_Image::InitImage(string tex_id)
{
	UIBase::Init();
	render_data_.tex_id = tex_id;
}
