#pragma once
#include "stdafx.h"
#include "DllMacro.h"
#include "DX11App.h"

#include <SpriteFont.h>



namespace reality
{
	enum E_Font
	{
		BASIC = 0,
		ROTUNDA = 1,
	};

	class DLL_API WriteMgr
	{
		SINGLETON(WriteMgr);
#define WRITER WriteMgr::GetInst()

	private:
		unique_ptr<SpriteBatch> sprite_batch_;
		unique_ptr<SpriteFont>	sprite_font_basic_;
		unique_ptr<SpriteFont>	sprite_font_rotunda_;
	public:
		void Init();

		void Draw(string text, E_Font font, XMFLOAT2 coord = { 0.0f, 0.0f }, float size = 1.0f, XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f });
	};
}

