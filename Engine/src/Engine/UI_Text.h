#pragma once
#include "UIBase.h"

namespace reality
{
	class DLL_API UI_Text : public UIBase
	{
	private:
		string		text_;
		float		size_;
		XMFLOAT4	color_;
	public:
		void InitText(string text, XMFLOAT2 pos = {0.0f, 0.0f}, float size = 1.0f, XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f });
	private:
		virtual void RenderThisUI() override;
	};
}


