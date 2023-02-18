#include "stdafx.h"
#include "Texture.h"
#include "DX11App.h"

bool KGCA41B::Texture::LoadTextureWIC(wstring filepath)
{
	HRESULT hr = CreateWICTextureFromFile(DX11APP->GetDevice(), filepath.c_str(), texture.GetAddressOf(), srv.GetAddressOf());
	if (FAILED(hr))
		return false;

	return true;
}
