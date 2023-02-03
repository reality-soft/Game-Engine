#include "Texture.h"
#include "DX11App.h"

bool KGCA41B::Texture::LoadTextureWIC(wstring filepath)
{
	res_list.push_back(nullptr);
	srv_list.push_back(nullptr);

	HRESULT hr = CreateWICTextureFromFile(DX11APP->GetDevice(), filepath.c_str(), &res_list.back(), &srv_list.back());
	if (FAILED(hr))
		return false;

	return true;
}
