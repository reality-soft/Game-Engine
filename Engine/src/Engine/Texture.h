#pragma once
#include "stdafx.h"
#include "DllMacro.h"

#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

namespace KGCA41B
{
	class DLL_API Texture
	{
	public:
		Texture() = default;
		~Texture() = default;

	public:
		bool LoadTextureWIC(wstring filepath);
		bool LoadTextureDDS(wstring filepath);
		ComPtr<ID3D11ShaderResourceView>	srv;
		D3D11_TEXTURE2D_DESC				texture_desc;

	};
}