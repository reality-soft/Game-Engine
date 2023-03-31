#include "stdafx.h"
#include "RenderTargetMgr.h"
#include "ResourceMgr.h"
#include "StaticShadowMap.h"

#define DEPTH_BIAS_D24_FLOAT(d) (d/(1/pow(2,24)))

bool reality::StaticShadowMap::Create(XMFLOAT3 light_position, string vs_id, string ps_id)
{
	XMVECTOR light_pos_vector = XMLoadFloat3(&light_position);
	XMMATRIX view_matrix = XMMatrixLookAtLH(light_pos_vector, XMVectorZero(), XMVectorSet(0, 1, 0, 0));
	XMMATRIX proj_matrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45), 1.0f, 5000.f, 15000.0f);

	cb_shadow_map_.data.shadow_view = XMMatrixTranspose(view_matrix);
	cb_shadow_map_.data.shadow_proj = XMMatrixTranspose(proj_matrix);

	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA subdata;

	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.ByteWidth = sizeof(CbShadowMap::Data);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	subdata.pSysMem = &cb_shadow_map_.data;

	HRESULT hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, cb_shadow_map_.buffer.GetAddressOf());
	if (FAILED(hr))
		return false;	

	vertex_shader_ = RESOURCE->UseResource<VertexShader>(vs_id);
	pixel_shader_ = RESOURCE->UseResource<PixelShader>(ps_id);

	if (vertex_shader_ == nullptr || pixel_shader_ == nullptr)
		return false;

	shadow_map_rt_ = RENDER_TARGET->MakeRT("ssm_rt", 4096, 4096).get();
	if (shadow_map_rt_ == nullptr)
		return false;

	D3D11_RASTERIZER_DESC rs_desc;
	ZeroMemory(&rs_desc, sizeof(rs_desc));

	rs_desc.FillMode = D3D11_FILL_SOLID;
	rs_desc.CullMode = D3D11_CULL_NONE;
	rs_desc.FrontCounterClockwise = false;
	rs_desc.DepthClipEnable = true;

	// Set depth bias parameters
	rs_desc.DepthBias = 1;
	rs_desc.SlopeScaledDepthBias = 10.f;
	rs_desc.DepthBiasClamp = 0.0f;

	// Create the rasterizer state
	hr = DX11APP->GetDevice()->CreateRasterizerState(&rs_desc, depth_bias_rs_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

void reality::StaticShadowMap::RenderLevelShadowMap(StaticMeshLevel* static_mesh_level)
{
	shadow_map_rt_->SetClearColor({ 0, 0, 0, 1 });
	shadow_map_rt_->SetRenderTarget();

	DX11APP->GetDeviceContext()->VSSetShader(nullptr, nullptr, 0);
	DX11APP->GetDeviceContext()->GSSetShader(nullptr, nullptr, 0);
	DX11APP->GetDeviceContext()->PSSetShader(nullptr, nullptr, 0);

	DX11APP->GetDeviceContext()->OMSetDepthStencilState(DX11APP->GetCommonStates()->DepthDefault(), 0xff);
	DX11APP->GetDeviceContext()->RSSetState(depth_bias_rs_.Get());

	DX11APP->GetDeviceContext()->VSSetShader(vertex_shader_->Get(), nullptr, 0);
	DX11APP->GetDeviceContext()->PSSetShader(pixel_shader_->Get(), nullptr, 0);
	DX11APP->GetDeviceContext()->IASetInputLayout(vertex_shader_->InputLayout());
	DX11APP->GetDeviceContext()->VSSetConstantBuffers(1, 1, cb_shadow_map_.buffer.GetAddressOf());
	for (auto& mesh : static_mesh_level->GetLevelMesh()->meshes)
	{
		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		DX11APP->GetDeviceContext()->Draw(mesh.vertices.size(), 0);
	}

	DX11APP->SetBackBufferRTV();
}

reality::CbShadowMap* reality::StaticShadowMap::GetShadowCb()
{
	return &cb_shadow_map_;
}
