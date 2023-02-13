#include "stdafx.h"
#include "Texture.h"
#include "DX11App.h"

bool KGCA41B::Texture::LoadTextureWIC(wstring filepath)
{
	ID3D11Resource* resource = nullptr;

	HRESULT hr = CreateWICTextureFromFile(DX11APP->GetDevice(), filepath.c_str(), &resource, srv.GetAddressOf());
	if (FAILED(hr))
		return false;

	resource->Release();

	return true;
}
