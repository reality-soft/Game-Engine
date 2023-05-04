#pragma once
#include "UIBase.h"

namespace reality
{
	class DLL_API UI_Image : public UIBase
	{
	public:
		void InitImage(string tex_id);
		void SetImage(string tex_id);
	};
}


