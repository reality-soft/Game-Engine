#include "stdafx.h"
#include "Surface.h"
#include "FileTransfer.h"
#include "ResourceMgr.h"

void KGCA41B::Surface::CreateSurface()
{
	pixel_shader = shared_ptr<PixelShader>(RESOURCE->UseResource<PixelShader>(shader_id));

	HRESULT hr;
	UINT slot_size = materials.size();

	texture_array_s0.resize(slot_size);

	int slot = 0;
	for (auto mat : materials)
	{
		for (auto tex : mat.texture_id)
		{
			Texture* texure = RESOURCE->UseResource<Texture>(tex);
			if (texure)
				texture_array_s0[slot] = texure->srv.Get();
		}
		slot++;
	}
}

void KGCA41B::Surface::SetSurface()
{
	if (pixel_shader.get() != nullptr){
		DX11APP->GetDeviceContext()->PSSetShader(pixel_shader.get()->Get(), 0, 0);
	}

	ID3D11SamplerState* sampler = DX11APP->GetCommonStates()->LinearWrap();
	DX11APP->GetDeviceContext()->PSSetSamplers(0, 0, &sampler);

	DX11APP->GetDeviceContext()->PSSetShaderResources(0, texture_array_s0.size(), texture_array_s0.data());
}

void KGCA41B::Surface::SaveDefinition(string filename)
{
	FileTransfer file_transfer("../../Contents/BinaryPackage/Surface/" + filename, WRITE);

	file_transfer.WriteBinary<string>(&shader_id, 1);

	int slot_size = materials.size();
	file_transfer.WriteBinary<int>(&slot_size, 1);
	for (auto& mat : materials)
	{
		file_transfer.WriteBinary<string>(mat.texture_id.data(), mat.texture_id.size());
	}
}

void KGCA41B::Surface::LoadDefinition(string filename)
{
	FileTransfer file_transfer("../../Contents/BinaryPackage/Surface/" + filename, READ);

	file_transfer.ReadBinary<string>(shader_id);

	int slot_size = 0;
	file_transfer.ReadBinary<int>(slot_size);
	materials.resize(slot_size);
	for (auto& mat : materials)
	{
		file_transfer.ReadBinary<string>(mat.texture_id);
	}
}
