#include "stdafx.h"
#include "WriteMgr.h"
#include "DX11App.h"
using namespace reality;

void WriteMgr::Init()
{
	sprite_batch_ = std::make_unique<SpriteBatch>(DX11APP->GetDeviceContext());
	sprite_font_ = std::make_unique<SpriteFont>(DX11APP->GetDevice(), L"../../Contents/SpriteFont/SpriteFont.spritefont");
}

void WriteMgr::Draw(string text, XMFLOAT2 coord, float size, XMFLOAT4 color)
{
	sprite_batch_->Begin();
	sprite_font_->DrawString(sprite_batch_.get(), to_mw(text).c_str(), coord, XMLoadFloat4(&color), 0, {}, size);
	sprite_batch_->End();
}
