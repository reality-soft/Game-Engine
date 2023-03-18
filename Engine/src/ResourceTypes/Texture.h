#pragma once
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

namespace reality
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
		ComPtr<ID3D11Texture2D> texture;
		D3D11_TEXTURE2D_DESC				texture_desc;
	};

	class DLL_API AlphaTexLayer
	{
	public:
		ComPtr<ID3D11Texture2D> alpha_texture;
		ComPtr<ID3D11ShaderResourceView> alpha_srv;
		vector<XMFLOAT4> alpha_data;

		int pixel_size;
		int texel_per_vertex;
		bool CreateAlphaTexture(int pixels, int _texel_per_vertex);
		bool ImportAlphaTexture(string filename);
		bool ExportAlphaTexture(string filename);

		void SetTexelAt(const XMVECTOR& pos, float radius, int current_layer, bool paint_mode);
	};
}