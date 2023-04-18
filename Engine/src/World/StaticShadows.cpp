#include "stdafx.h"
#include "PostProcess.h"
#include "RenderTargetMgr.h"
#include "ResourceMgr.h"
#include "StaticShadows.h"

bool reality::SingleShadow::Init(XMFLOAT2 near_far, XMFLOAT2 dmap_size, XMFLOAT2 smap_size, string dmap_vs, string smap_vs, string smap_ps)
{
	projection_near_far_ = near_far;

	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA subdata;

	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	// Shaders
	depth_map_vs_ = RESOURCE->UseResource<VertexShader>(dmap_vs);
	shadow_map_vs_ = RESOURCE->UseResource<VertexShader>(smap_vs);
	shadow_map_ps_ = RESOURCE->UseResource<PixelShader>(smap_ps);

	if (depth_map_vs_ == nullptr || shadow_map_vs_ == nullptr || shadow_map_ps_ == nullptr)
		return false;

	// Render Targets
	depth_map_rt_ = RENDER_TARGET->MakeRT("dmap_rt", dmap_size.x, dmap_size.y).get();
	if (depth_map_rt_ == nullptr)
		return false;

	shadow_map_rt_ = RENDER_TARGET->MakeRT("smap", smap_size.x, smap_size.y).get();
	if (shadow_map_rt_ == nullptr)
		return false;

	//Depth Bias Rasterizer
	D3D11_RASTERIZER_DESC rs_desc;
	ZeroMemory(&rs_desc, sizeof(rs_desc));

	rs_desc.FillMode = D3D11_FILL_SOLID;
	rs_desc.CullMode = D3D11_CULL_NONE;
	rs_desc.FrontCounterClockwise = false;
	rs_desc.DepthClipEnable = false;

	rs_desc.DepthBias = 1;
	rs_desc.SlopeScaledDepthBias = 1;
	rs_desc.DepthBiasClamp = 0;

	HRESULT hr = DX11APP->GetDevice()->CreateRasterizerState(&rs_desc, &depth_bias_rs_);
	if (FAILED(hr))
		return false;

	// Create Buffer
	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.ByteWidth = sizeof(CbSingleShadow::Data);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	subdata.pSysMem = &cb_single_shadow_.data;

	hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, cb_single_shadow_.buffer.GetAddressOf());
	if (FAILED(hr))
		return false;

	// Samper
	sampler_clamp_ = DX11APP->GetCommonStates()->LinearClamp();

	return true;
}

void reality::SingleShadow::RenderDepthMap(XMVECTOR light_pos, XMVECTOR light_look)
{
	XMMATRIX light_view = XMMatrixLookAtLH(light_pos, light_look, XMVectorSet(0, 1, 0, 0));
	XMMATRIX light_proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(90), 1.0f, projection_near_far_.x, projection_near_far_.y);
	cb_single_shadow_.data.shadow_view_proj = XMMatrixTranspose(light_view * light_proj);// *XMMatrixTranspose(light_proj);
	DX11APP->GetDeviceContext()->UpdateSubresource(cb_single_shadow_.buffer.Get(), 0, 0, &cb_single_shadow_.data, 0, 0);

	//Render
	depth_map_rt_->SetClearColor({ 0, 0, 0, 1 });
	depth_map_rt_->SetRenderTarget();

	DX11APP->GetDeviceContext()->VSSetShader(nullptr, nullptr, 0);
	DX11APP->GetDeviceContext()->GSSetShader(nullptr, nullptr, 0);
	DX11APP->GetDeviceContext()->PSSetShader(nullptr, nullptr, 0);

	DX11APP->GetDeviceContext()->OMSetDepthStencilState(DX11APP->GetCommonStates()->DepthDefault(), 0xff);
	DX11APP->GetDeviceContext()->RSSetState(depth_bias_rs_);

	DX11APP->GetDeviceContext()->VSSetConstantBuffers(1, 1, cb_single_shadow_.buffer.GetAddressOf());

	DX11APP->GetDeviceContext()->VSSetShader(depth_map_vs_->Get(), nullptr, 0);
	DX11APP->GetDeviceContext()->IASetInputLayout(depth_map_vs_->InputLayout());

	for (auto& mesh : static_mesh_level_->GetLevelMesh()->meshes)
	{
		if (mesh.mesh_name == "Collision")
			continue;

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		DX11APP->GetDeviceContext()->Draw(mesh.vertices.size(), 0);
	}

	DX11APP->SetBackBufferRTV();
}

void reality::SingleShadow::RenderShadowMap()
{

	// Blur Shadow Texture
	shadow_map_rt_->SetClearColor({ 0, 0, 0, 1 });
	shadow_map_rt_->SetRenderTarget();

	DX11APP->GetDeviceContext()->VSSetShader(nullptr, nullptr, 0);
	DX11APP->GetDeviceContext()->GSSetShader(nullptr, nullptr, 0);
	DX11APP->GetDeviceContext()->PSSetShader(nullptr, nullptr, 0);
	
	DX11APP->GetDeviceContext()->OMSetDepthStencilState(DX11APP->GetCommonStates()->DepthDefault(), 0xff);
	DX11APP->GetDeviceContext()->RSSetState(DX11APP->GetCommonStates()->CullNone());

	DX11APP->GetDeviceContext()->PSSetSamplers(0, 1, &sampler_clamp_);
	DX11APP->GetDeviceContext()->PSSetShaderResources(0, 1, depth_map_rt_->depth_stencil_view_srv_.GetAddressOf());
	DX11APP->GetDeviceContext()->PSSetShader(shadow_map_ps_->Get(), 0, 0);
	
	DX11APP->GetDeviceContext()->VSSetShader(shadow_map_vs_->Get(), nullptr, 0);
	DX11APP->GetDeviceContext()->VSSetConstantBuffers(1, 1, cb_single_shadow_.buffer.GetAddressOf());

	DX11APP->GetDeviceContext()->IASetInputLayout(static_mesh_level_->GetVertexShader()->InputLayout());

	for (auto& mesh : static_mesh_level_->GetLevelMesh()->meshes)
	{
		if (mesh.mesh_name == "Collision")
			continue;

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		DX11APP->GetDeviceContext()->Draw(mesh.vertices.size(), 0);
	}



	DX11APP->SetBackBufferRTV();
}

ID3D11ShaderResourceView* reality::SingleShadow::GetDepthMapSRV()
{
	return depth_map_rt_->depth_stencil_view_srv_.Get();
}

ID3D11ShaderResourceView* reality::SingleShadow::GetRTSRV()
{
	return shadow_map_rt_->render_target_view_srv_.Get();
}

void reality::SingleShadow::SetShadowMapSRV()
{
	DX11APP->GetDeviceContext()->PSSetShaderResources(8, 1, shadow_map_rt_->render_target_view_srv_.GetAddressOf());
	DX11APP->GetDeviceContext()->PSSetSamplers(1, 1, &sampler_clamp_);
}

bool reality::MultipleShadows::Init(XMFLOAT2 near_far, XMFLOAT2 dmap_size, XMFLOAT2 smap_size, string dmap_vs, string smap_vs, string smap_ps)
{
	projection_near_far_ = near_far;

	// Shaders
	depth_map_vs_ = RESOURCE->UseResource<VertexShader>(dmap_vs);
	shadow_map_vs_ = RESOURCE->UseResource<VertexShader>(smap_vs);
	shadow_map_ps_ = RESOURCE->UseResource<PixelShader>(smap_ps);

	if (depth_map_vs_ == nullptr || shadow_map_vs_ == nullptr || shadow_map_ps_ == nullptr)
		return false;

	// Render Targets
	depth_map_rt_ = RENDER_TARGET->MakeRT("dmap_rt", dmap_size.x, dmap_size.y).get();
	if (depth_map_rt_ == nullptr)
		return false;

	shadow_map_rt_ = RENDER_TARGET->MakeRT("smap", smap_size.x, smap_size.y).get();
	if (shadow_map_rt_ == nullptr)
		return false;

	//Depth Bias Rasterizer
	D3D11_RASTERIZER_DESC rs_desc;
	ZeroMemory(&rs_desc, sizeof(rs_desc));

	rs_desc.FillMode = D3D11_FILL_SOLID;
	rs_desc.CullMode = D3D11_CULL_NONE;
	rs_desc.FrontCounterClockwise = false;
	rs_desc.DepthClipEnable = false;

	rs_desc.DepthBias = 1;
	rs_desc.SlopeScaledDepthBias = 1;
	rs_desc.DepthBiasClamp = 0;

	HRESULT hr = DX11APP->GetDevice()->CreateRasterizerState(&rs_desc, &depth_bias_rs_);
	if (FAILED(hr))
		return false;
	
	// TotalShadowBuffer
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA subdata;

	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.ByteWidth = sizeof(CbMultipleShadows::Data);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	subdata.pSysMem = &cb_total_shadows_.data;

	hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, cb_total_shadows_.buffer.GetAddressOf());
	if (FAILED(hr))
		return false;

	// Samper
	sampler_clamp_ = DX11APP->GetCommonStates()->LinearClamp();

	return true;
}

bool reality::MultipleShadows::CreateDepthMap(XMVECTOR light_pos, XMVECTOR light_dir)
{
	XMMATRIX light_view = XMMatrixLookToLH(light_pos, light_dir, XMVectorSet(0, 1, 0, 0));
	XMMATRIX light_proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(90), 1.0f, projection_near_far_.x, projection_near_far_.y);

	// Create Buffer
	CbSingleShadow single_shadow_buffer_;
	single_shadow_buffer_.data.shadow_view_proj = XMMatrixTranspose(light_view) * XMMatrixTranspose(light_proj);

	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA subdata;

	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.ByteWidth = sizeof(CbSingleShadow::Data);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	subdata.pSysMem = &single_shadow_buffer_.data;

	HRESULT hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, single_shadow_buffer_.buffer.GetAddressOf());
	if (FAILED(hr))
		return false;
	
	// Push To Total Shadow Buffer
	if (cb_total_shadows_.index > 9)
		return false;

	cb_total_shadows_.data.shadow_view_proj[cb_total_shadows_.index++] = single_shadow_buffer_.data.shadow_view_proj;
	DX11APP->GetDeviceContext()->UpdateSubresource(cb_total_shadows_.buffer.Get(), 0, 0, &cb_total_shadows_.data, 0, 0);

	depth_map_rt_->SetClearColor({ 0, 0, 0, 1 });
	depth_map_rt_->SetRenderTarget();

	DX11APP->GetDeviceContext()->VSSetShader(nullptr, nullptr, 0);
	DX11APP->GetDeviceContext()->GSSetShader(nullptr, nullptr, 0);
	DX11APP->GetDeviceContext()->PSSetShader(nullptr, nullptr, 0);

	DX11APP->GetDeviceContext()->OMSetDepthStencilState(DX11APP->GetCommonStates()->DepthDefault(), 0xff);
	DX11APP->GetDeviceContext()->RSSetState(depth_bias_rs_);
	DX11APP->GetDeviceContext()->VSSetConstantBuffers(1, 1, single_shadow_buffer_.buffer.GetAddressOf());

	DX11APP->GetDeviceContext()->IASetInputLayout(depth_map_vs_->InputLayout());
	DX11APP->GetDeviceContext()->VSSetShader(depth_map_vs_->Get(), nullptr, 0);

	for (auto& mesh : static_mesh_level_->GetLevelMesh()->meshes)
	{
		if (mesh.mesh_name == "Collision")
			continue;

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		DX11APP->GetDeviceContext()->Draw(mesh.vertices.size(), 0);
	}

	DX11APP->SetBackBufferRTV();

	depth_map_textures_.push_back(depth_map_rt_->depth_stencil_view_srv_.Get());

	return false;
}

void reality::MultipleShadows::RenderShadowMap()
{
}
