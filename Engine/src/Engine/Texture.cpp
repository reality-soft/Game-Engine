#include "stdafx.h"
#include "Texture.h"
#include "DX11App.h"

bool reality::Texture::LoadTextureWIC(wstring filepath)
{
	HRESULT hr;

	hr = CreateWICTextureFromFile(
		DX11APP->GetDevice(),
		DX11APP->GetDeviceContext(),
		filepath.c_str(),
		(ID3D11Resource**)texture.GetAddressOf(), srv.GetAddressOf());

	if (FAILED(hr))
		return false;

	texture.Get()->QueryInterface<ID3D11Texture2D>(&texture);
	if (texture)
		texture->GetDesc(&texture_desc);

	texture->Release();

	return true;
}

bool reality::Texture::LoadTextureDDS(wstring filepath)
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

bool reality::AlphaTexLayer::CreateAlphaTexture(int pixels, int _texel_per_vertex)
{
	pixel_size = pixels;
	texel_per_vertex = _texel_per_vertex;
	alpha_data.resize(pixels * pixels);
	ZeroMemory(alpha_data.data(), sizeof(alpha_data));

	HRESULT hr;

	D3D11_TEXTURE2D_DESC tex_desc;
	D3D11_SUBRESOURCE_DATA subdata;
	ZeroMemory(&tex_desc, sizeof(tex_desc));
	ZeroMemory(&subdata, sizeof(subdata));

	tex_desc.Width = pixel_size;
	tex_desc.Height = pixel_size;
	tex_desc.MipLevels = tex_desc.ArraySize = 1;
	tex_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tex_desc.SampleDesc.Count = 1;
	tex_desc.Usage = D3D11_USAGE_DEFAULT;
	tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tex_desc.CPUAccessFlags = 0;
	tex_desc.MiscFlags = 0;

	subdata.pSysMem = alpha_data.data();
	subdata.SysMemPitch = sizeof(XMFLOAT4) * pixels;

	hr = DX11APP->GetDevice()->CreateTexture2D(&tex_desc, &subdata, alpha_texture.GetAddressOf());
	if (FAILED(hr))
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
	ZeroMemory(&srv_desc, sizeof(srv_desc));
	srv_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MipLevels = 1;

	hr = DX11APP->GetDevice()->CreateShaderResourceView(alpha_texture.Get(), &srv_desc, alpha_srv.GetAddressOf());
	if (FAILED(hr))
		return false;

	return true;
}

void reality::AlphaTexLayer::SetTexelAt(const XMVECTOR& pos, float radius, int current_layer, bool paint_mode)
{
	XMFLOAT2 texel_coord;

	texel_coord.x = (pos.m128_f32[0] / 1.5 + 64) * 8;
	texel_coord.y = fabs((pos.m128_f32[2] / 1.5 - 64)) * 8;

	for (int i = 0; i < pixel_size; ++i)
	{
		for (int j = 0; j < pixel_size; ++j)
		{
			float distance = Distance(XMVectorSet(i, 0, j, 0), XMVectorSet(texel_coord.x, 0, texel_coord.y, 0));
			float strength = 1.0f - (distance / (radius * 8));

			if (paint_mode == false)
				strength *= -1.0f;

			if (distance <= radius * 8)
			{
				switch (current_layer)
				{
				case 1:
					alpha_data[j * pixel_size + i].x += strength;
					alpha_data[j * pixel_size + i].x = min(alpha_data[j * pixel_size + i].x, 1.0f);
					alpha_data[j * pixel_size + i].x = max(alpha_data[j * pixel_size + i].x, 0.0f);
					break;

				case 2:
					alpha_data[j * pixel_size + i].y += strength;
					alpha_data[j * pixel_size + i].y = min(alpha_data[j * pixel_size + i].y, 1.0f);
					alpha_data[j * pixel_size + i].y = max(alpha_data[j * pixel_size + i].y, 0.0f);
					break;

				case 3:
					alpha_data[j * pixel_size + i].z += strength;
					alpha_data[j * pixel_size + i].z = min(alpha_data[j * pixel_size + i].z, 1.0f);
					alpha_data[j * pixel_size + i].z = max(alpha_data[j * pixel_size + i].z, 0.0f);
					break;

				case 4:
					alpha_data[j * pixel_size + i].w += strength;
					alpha_data[j * pixel_size + i].w = min(alpha_data[j * pixel_size + i].w, 1.0f);
					alpha_data[j * pixel_size + i].w = max(alpha_data[j * pixel_size + i].w, 0.0f);
					break;
				}
				
				
				
				

				
				
				
				
			}
		}
	}
}
