#pragma once
#include "UIBase.h"
#include "WriteMgr.h"

namespace reality
{
	class DLL_API UI_Text : public UIBase
	{
	private:
		string		text_;
		E_Font		font_;
		float		size_;
		XMFLOAT4	color_;
	public:
		void InitText(string text, E_Font font, XMFLOAT2 pos = {0.0f, 0.0f}, float size = 1.0f, XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f });
		void SetText(string text);
		void SetFont(E_Font font);
		string GetText() { return text_; }
	private:
		virtual void RenderThisUI() override;
	};
}


