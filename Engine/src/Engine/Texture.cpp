#include "Texture.h"
#include "DX11App.h"

KGCA41B::Texture::~Texture()
{
	for (auto srv : srv_list)
	{
		srv->Release();
	}
}

bool KGCA41B::Texture::LoadTextureWIC(wstring filepath)
{
	ID3D11Resource* resource = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;

	HRESULT hr = CreateWICTextureFromFile(DX11APP->GetDevice(), filepath.c_str(), &resource, &srv);
	if (FAILED(hr))
		return false;

	srv_list.push_back(srv);

	resource->Release();

	return true;
}
