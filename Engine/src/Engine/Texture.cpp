#include "stdafx.h"
#include "Texture.h"
#include "DX11App.h"

bool KGCA41B::Texture::LoadTextureWIC(wstring filepath)
{
	HRESULT hr = CreateWICTextureFromFile(DX11APP->GetDevice(), filepath.c_str(), resource.GetAddressOf(), srv.GetAddressOf());
	if (FAILED(hr))
		return false;

	ID3D11Texture2D* texture;
	resource.Get()->QueryInterface<ID3D11Texture2D>(&texture);
	if (texture)
		texture->GetDesc(&texture_desc);

	texture->Release();

	if (filepath.find(L"DF") != wstring::npos)
	{
		texture_type = DIFFUSE;
	}
	if (filepath.find(L"NM") != wstring::npos)
	{
		texture_type = NORMALMAP;
	}
	if (filepath.find(L"MT") != wstring::npos)
	{
		texture_type = METALIC;
	}
	if (filepath.find(L"RN") != wstring::npos)
	{
		texture_type = ROUGHNESS;
	}
	if (filepath.find(L"SP") != wstring::npos)
	{
		texture_type = SPECULAR;
	}
	if (filepath.find(L"AO") != wstring::npos)
	{
		texture_type = AMBIENT;
	}
	if (filepath.find(L"OP") != wstring::npos)
	{
		texture_type = OPACITY;
	}

	return true;
}