#include "stdafx.h"
#include "Texture.h"
#include "DX11App.h"

bool KGCA41B::Texture::LoadTextureWIC(wstring filepath)
{
	ID3D11Resource* resource = nullptr;

	HRESULT hr = CreateWICTextureFromFile(DX11APP->GetDevice(), filepath.c_str(), &resource, srv.GetAddressOf());
	if (FAILED(hr))
		return false;

	ID3D11Texture2D* texture;
	resource->QueryInterface<ID3D11Texture2D>(&texture);
	if (texture)
		texture->GetDesc(&texture_desc);

	texture->Release();

	return true;
}

bool KGCA41B::Texture::LoadTextureDDS(wstring filepath)
{
	ID3D11Resource* resource = nullptr;
	HRESULT hr = CreateDDSTextureFromFile(DX11APP->GetDevice(), filepath.c_str(), &resource, srv.GetAddressOf());
	if (FAILED(hr))
		return false;

	ID3D11Texture2D* texture;
	resource->QueryInterface<ID3D11Texture2D>(&texture);
	if (texture)
		texture->GetDesc(&texture_desc);

	texture->Release();

	return true;
}
