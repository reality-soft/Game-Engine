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
		EUROCINE = 2,
		ALTONE = 3,
		RENOGARE = 4,
		ROBOTO = 5,
	};

	class DLL_API WriteMgr
	{
		SINGLETON(WriteMgr);
#define WRITER WriteMgr::GetInst()

	private:
		unique_ptr<SpriteBatch> sprite_batch_;
		unique_ptr<SpriteFont>	sprite_font_basic_;
		unique_ptr<SpriteFont>	sprite_font_rotunda_;
		unique_ptr<SpriteFont>	sprite_font_eurocine_;
		unique_ptr<SpriteFont>	sprite_font_altone_;
		unique_ptr<SpriteFont>	sprite_font_renogare_;
		unique_ptr<SpriteFont>	sprite_font_roboto_;
	public:
		void Init();

		void Draw(string text, E_Font font, XMFLOAT2 coord = { 0.0f, 0.0f }, float size = 1.0f, XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f });
	};
}

