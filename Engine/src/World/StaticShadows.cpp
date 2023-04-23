#include "stdafx.h"
#include "PostProcess.h"
#include "RenderTargetMgr.h"
#include "ResourceMgr.h"
#include "StaticShadows.h"
#include "wincodec.h"

bool reality::CreateDepthBiasRS(int DepthBias, float SlopeScaledDepthBias, float DepthBiasClamp, ID3D11RasterizerState** rs)
{
	//Depth Bias Rasterizer
	D3D11_RASTERIZER_DESC rs_desc;
	ZeroMemory(&rs_desc, sizeof(rs_desc));

	rs_desc.FillMode = D3D11_FILL_SOLID;
	rs_desc.CullMode = D3D11_CULL_NONE;
	rs_desc.FrontCounterClockwise = false;
	rs_desc.DepthClipEnable = true;

	rs_desc.DepthBias = DepthBias;
	rs_desc.SlopeScaledDepthBias = SlopeScaledDepthBias;
	rs_desc.DepthBiasClamp = DepthBiasClamp;

	HRESULT hr = DX11APP->GetDevice()->CreateRasterizerState(&rs_desc, rs);
	if (FAILED(hr))
		return false;

	return true;
}

bool reality::ProjectionShadow::Init(XMFLOAT2 near_far, XMFLOAT2 dmap_size, string dmap_vs)
{
	projection_near_far_ = near_far;

	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA subdata;

	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	// Shaders
	depth_map_vs_ = RESOURCE->UseResource<VertexShader>(dmap_vs);

	if (depth_map_vs_ == nullptr)
		return false;

	// Render Targets
	depth_map_rt_ = RENDER_TARGET->MakeRT("dmap_rt", dmap_size.x, dmap_size.y).get();
	if (depth_map_rt_ == nullptr)
		return false;

	CreateDepthBiasRS(1, 1, 0, &depth_bias_rs_);

	// Create Buffer
	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.ByteWidth = sizeof(CbProjectionShadow::Data);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	subdata.pSysMem = &cb_shadow_.data;

	HRESULT hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, cb_shadow_.buffer.GetAddressOf());
	if (FAILED(hr))
		return false;

	// Samper
	sampler_clamp_ = DX11APP->GetCommonStates()->LinearClamp();

	return true;
}

bool reality::ProjectionShadow::CreateDepthMap(StaticMeshLevel* level_to_render, XMVECTOR light_pos, XMVECTOR light_look)
{
	XMMATRIX light_view = XMMatrixLookAtLH(light_pos, light_look, XMVectorSet(0, 1, 0, 0));
	XMMATRIX light_proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(90), 1.0f, projection_near_far_.x, projection_near_far_.y);
	cb_shadow_.data.shadow_view_proj = XMMatrixTranspose(light_view * light_proj);
	DX11APP->GetDeviceContext()->UpdateSubresource(cb_shadow_.buffer.Get(), 0, 0, &cb_shadow_.data, 0, 0);

	//Render
	depth_map_rt_->SetClearColor({ 0, 0, 0, 1 });
	depth_map_rt_->SetRenderTarget();

	DX11APP->GetDeviceContext()->VSSetShader(nullptr, nullptr, 0);
	DX11APP->GetDeviceContext()->GSSetShader(nullptr, nullptr, 0);
	DX11APP->GetDeviceContext()->PSSetShader(nullptr, nullptr, 0);

	DX11APP->GetDeviceContext()->OMSetDepthStencilState(DX11APP->GetCommonStates()->DepthDefault(), 0xff);
	DX11APP->GetDeviceContext()->RSSetState(depth_bias_rs_);

	DX11APP->GetDeviceContext()->VSSetConstantBuffers(1, 1, cb_shadow_.buffer.GetAddressOf());

	DX11APP->GetDeviceContext()->VSSetShader(depth_map_vs_->Get(), nullptr, 0);
	DX11APP->GetDeviceContext()->IASetInputLayout(depth_map_vs_->InputLayout());

	for (auto& mesh : level_to_render->GetLevelMesh()->meshes)
	{
		if (mesh.mesh_name == "Collision")
			continue;

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		DX11APP->GetDeviceContext()->Draw(mesh.vertices.size(), 0);
	}

	DX11APP->SetBackBufferRTV();

	return true;
}

void reality::ProjectionShadow::SetDepthMapSRV()
{
	DX11APP->GetDeviceContext()->VSSetConstantBuffers(1, 1, cb_shadow_.buffer.GetAddressOf());
	DX11APP->GetDeviceContext()->PSSetShaderResources(8, 1, depth_map_rt_->depth_stencil_view_srv_.GetAddressOf());
	DX11APP->GetDeviceContext()->PSSetSamplers(1, 1, &sampler_clamp_);
}

ID3D11ShaderResourceView* reality::ProjectionShadow::GetDepthMapSRV()
{
	return depth_map_rt_->depth_stencil_view_srv_.Get();
}



bool reality::CubemapShadow::Init(XMFLOAT2 near_far, XMFLOAT2 dmap_size, string dmap_vs)
{
	projection_near_far_ = near_far;

	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA subdata;

	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	// Shaders
	depth_map_vs_ = RESOURCE->UseResource<VertexShader>(dmap_vs);

	if (depth_map_vs_ == nullptr)
		return false;

	// Render Targets

	for (int i = 0; i < 6; ++i)
	{
		depth_map_rts_[i] = RENDER_TARGET->MakeRT("dmap_rt", dmap_size.x, dmap_size.y).get();
		if (depth_map_rts_[i] == nullptr)
			return false;
	}
	

	CreateDepthBiasRS(1, 1, 0, &depth_bias_rs_);

	// Create Buffer

	for (int i = 0; i < 6; ++i)
	{
		ZeroMemory(&desc, sizeof(desc));
		ZeroMemory(&subdata, sizeof(subdata));

		desc.ByteWidth = sizeof(CbProjectionShadow::Data);
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		subdata.pSysMem = &cb_shadow_[i].data;

		HRESULT hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, cb_shadow_[i].buffer.GetAddressOf());
		if (FAILED(hr))
			return false;
	}

	// Samper
	sampler_clamp_ = DX11APP->GetCommonStates()->LinearClamp();

	return true;
}

bool reality::CubemapShadow::CreateDepthMap(StaticMeshLevel* level_to_render, XMVECTOR light_pos)
{
	XMMATRIX light_proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(90), 1.0f, projection_near_far_.x, projection_near_far_.y);
	XMMATRIX light_view[6];

	light_view[0] = XMMatrixLookAtLH(light_pos, light_pos + XMVectorSet( 1,  0,  0, NULL), XMVectorSet(0,  1,  0, NULL));
	light_view[1] = XMMatrixLookAtLH(light_pos, light_pos + XMVectorSet( 0,  1,  0, NULL), XMVectorSet(0,  0, -1, NULL));
	light_view[2] = XMMatrixLookAtLH(light_pos, light_pos + XMVectorSet( 0,  0,  1, NULL), XMVectorSet(0,  1,  0, NULL));
	light_view[3] = XMMatrixLookAtLH(light_pos, light_pos + XMVectorSet(-1,  0,  0, NULL), XMVectorSet(0,  1,  0, NULL));
	light_view[4] = XMMatrixLookAtLH(light_pos, light_pos + XMVectorSet( 0, -1,  0, NULL), XMVectorSet(0,  1,  1, NULL));
	light_view[5] = XMMatrixLookAtLH(light_pos, light_pos + XMVectorSet( 0,  0, -1, NULL), XMVectorSet(0,  1,  0, NULL));

	for (int i = 0; i < 6; ++i)
	{
		cb_shadow_[i].data.shadow_view_proj = XMMatrixTranspose(light_view[i] * light_proj);
		DX11APP->GetDeviceContext()->UpdateSubresource(cb_shadow_[i].buffer.Get(), 0, 0, &cb_shadow_[i].data, 0, 0);
	}

	DX11APP->GetDeviceContext()->VSSetShader(nullptr, nullptr, 0);
	DX11APP->GetDeviceContext()->GSSetShader(nullptr, nullptr, 0);
	DX11APP->GetDeviceContext()->PSSetShader(nullptr, nullptr, 0);

	for (int i = 0; i < 6; ++i)
	{
		DX11APP->GetDeviceContext()->OMSetDepthStencilState(DX11APP->GetCommonStates()->DepthDefault(), 0xff);
		DX11APP->GetDeviceContext()->RSSetState(depth_bias_rs_);
		DX11APP->GetDeviceContext()->VSSetShader(depth_map_vs_->Get(), nullptr, 0);
		DX11APP->GetDeviceContext()->IASetInputLayout(depth_map_vs_->InputLayout());

		depth_map_rts_[i]->SetClearColor({ 0, 0, 0, 1 });
		depth_map_rts_[i]->SetRenderTarget();
		DX11APP->GetDeviceContext()->VSSetConstantBuffers(1, 1, cb_shadow_[i].buffer.GetAddressOf());

		for (auto& mesh : level_to_render->GetLevelMesh()->meshes)
		{
			if (mesh.mesh_name == "Collision")
				continue;

			UINT stride = sizeof(Vertex);
			UINT offset = 0;

			DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
			DX11APP->GetDeviceContext()->Draw(mesh.vertices.size(), 0);
		}

		depth_srvs[i] = depth_map_rts_[i]->depth_stencil_view_srv_.Get();
		DX11APP->SetBackBufferRTV();
	}

	for (int i = 0; i < 6; ++i)
	{
		cb_cubemap_shadow_.data.shadow_view_proj[i] = cb_shadow_[i].data.shadow_view_proj;
	}

	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA subdata;

	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.ByteWidth = sizeof(CbCubeMapShadow::Data);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	subdata.pSysMem = &cb_cubemap_shadow_.data;

	HRESULT hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, cb_cubemap_shadow_.buffer.GetAddressOf());
	if (FAILED(hr))
		return false;

	return true;
}

void reality::CubemapShadow::CreateTextureFile()
{
	for (int i = 0; i < 6; ++i)
	{
		ID3D11Resource* pResource;
		depth_srvs[i]->GetResource(&pResource);

		// Create a staging texture with the same format and size as the original texture
		D3D11_TEXTURE2D_DESC desc;
		ID3D11Texture2D* pStagingTexture = nullptr;
		((ID3D11Texture2D*)pResource)->GetDesc(&desc);
		desc.Usage = D3D11_USAGE_STAGING;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.BindFlags = 0;
		DX11APP->GetDevice()->CreateTexture2D(&desc, nullptr, &pStagingTexture);

		// Copy the contents of the original texture to the staging texture
		DX11APP->GetDeviceContext()->CopyResource(pStagingTexture, pResource);

		// Save the contents of the staging texture to a file
		//SaveWICTextureToFile(DX11APP->GetDeviceContext(), pStagingTexture, GUID_ContainerFormatJpeg)

		// Release resources
		//pStagingTexture->Release();
		//pResource->Release();
	}
}

void reality::CubemapShadow::SetDepthMapSRV(int slot)
{
	DX11APP->GetDeviceContext()->VSSetConstantBuffers(2, 1, cb_cubemap_shadow_.buffer.GetAddressOf());
	DX11APP->GetDeviceContext()->PSSetShaderResources(slot, 6, depth_srvs);
	DX11APP->GetDeviceContext()->PSSetSamplers(1, 1, &sampler_clamp_);
}
