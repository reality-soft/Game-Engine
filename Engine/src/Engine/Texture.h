#pragma once
#include "DllMacro.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

namespace KGCA41B
{
	enum
	{
		DIFFUSE,
		NORMALMAP,
		METALIC,
		ROUGHNESS,
		SPECULAR,
		AMBIENT,
		OPACITY

	} typedef TEXTYPE;

	class DLL_API Texture
	{
	public:
		Texture() = default;
		~Texture() = default;

	public:
		bool LoadTextureWIC(wstring filepath);
		TEXTYPE texture_type;
		ComPtr<ID3D11ShaderResourceView>	srv;
		ComPtr<ID3D11Resource> resource;
		D3D11_TEXTURE2D_DESC				texture_desc;
	};
}