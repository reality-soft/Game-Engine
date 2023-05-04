#include "stdafx.h"
#include "WriteMgr.h"
#include "DX11App.h"
using namespace reality;

void WriteMgr::Init()
{
	sprite_batch_ = std::make_unique<SpriteBatch>(DX11APP->GetDeviceContext());
	sprite_font_basic_ = std::make_unique<SpriteFont>(DX11APP->GetDevice(), L"../../Contents/SpriteFont/Basic.spritefont");
	sprite_font_rotunda_ = std::make_unique<SpriteFont>(DX11APP->GetDevice(), L"../../Contents/SpriteFont/BerryRotunda.spritefont");
	sprite_font_eurocine_ = std::make_unique<SpriteFont>(DX11APP->GetDevice(), L"../../Contents/SpriteFont/Eurocine.spritefont");
	sprite_font_altone_ = std::make_unique<SpriteFont>(DX11APP->GetDevice(), L"../../Contents/SpriteFont/Altone.spritefont");
	sprite_font_renogare_ = std::make_unique<SpriteFont>(DX11APP->GetDevice(), L"../../Contents/SpriteFont/Renogare.spritefont");
	sprite_font_roboto_ = std::make_unique<SpriteFont>(DX11APP->GetDevice(), L"../../Contents/SpriteFont/Roboto.spritefont");
}

void WriteMgr::Draw(string text, E_Font font, XMFLOAT2 coord, float size, XMFLOAT4 color)
{
	sprite_batch_->Begin();
	switch (font)
	{
	case BASIC:
		sprite_font_basic_->DrawString(sprite_batch_.get(), to_mw(text).c_str(), coord, XMLoadFloat4(&color), 0, {}, size);
		break;
	case ROTUNDA:
		sprite_font_rotunda_->DrawString(sprite_batch_.get(), to_mw(text).c_str(), coord, XMLoadFloat4(&color), 0, {}, size);
		break;
	case EUROCINE:
		sprite_font_eurocine_->DrawString(sprite_batch_.get(), to_mw(text).c_str(), coord, XMLoadFloat4(&color), 0, {}, size);
		break;
	case ALTONE:
		sprite_font_altone_->DrawString(sprite_batch_.get(), to_mw(text).c_str(), coord, XMLoadFloat4(&color), 0, {}, size);
		break;
	case RENOGARE:
		sprite_font_renogare_->DrawString(sprite_batch_.get(), to_mw(text).c_str(), coord, XMLoadFloat4(&color), 0, {}, size);
		break;
	case ROBOTO:
		sprite_font_roboto_->DrawString(sprite_batch_.get(), to_mw(text).c_str(), coord, XMLoadFloat4(&color), 0, {}, size);
		break;
	}
	sprite_batch_->End();
}
