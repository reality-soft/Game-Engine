#pragma once
#include "stdafx.h"
#include "DllMacro.h"
#include "DX11App.h"

#include <SpriteFont.h>

namespace reality
{
	class DLL_API WriteMgr
	{
		SINGLETON(WriteMgr);
#define WRITER WriteMgr::GetInst()

	private:
		unique_ptr<SpriteBatch> sprite_batch_;
		unique_ptr<SpriteFont>	sprite_font_;
	public:
		void Init();

		void Draw(string text, XMFLOAT2 coord, float size = 1.0f, XMFLOAT4 color = {1.0f, 1.0f, 1.0f, 1.0f});
	};
}

