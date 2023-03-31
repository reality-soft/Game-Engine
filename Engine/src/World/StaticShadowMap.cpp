#include "stdafx.h"
#include "RenderTargetMgr.h"
#include "ResourceMgr.h"
#include "StaticShadowMap.h"

bool reality::StaticShadowMap::Create(XMVECTOR light_position, string vs_id, string ps_id)
{
	XMMATRIX view_matrix = XMMatrixLookAtLH(light_position, XMVectorZero(), XMVectorSet(0, 1, 0, 0));
	XMMATRIX proj_matrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45), 1.0f, 1.0f, 100000.0f);

	shadow_matrix_ = XMMatrixInverse(nullptr, view_matrix * proj_matrix);

	vertex_shader_ = RESOURCE->UseResource<VertexShader>(vs_id);
	pixel_shader_ = RESOURCE->UseResource<PixelShader>(ps_id);

	if (vertex_shader_ == nullptr || pixel_shader_ == nullptr)
		return false;

	shadow_map_rt_ = RENDER_TARGET->MakeRT("ssm_rt", 4096, 4096).get();
	if (shadow_map_rt_ == nullptr)
		return false;

	return true;
}

void reality::StaticShadowMap::RenderLevelShadowMap(StaticMeshLevel* static_mesh_level)
{
	shadow_map_rt_->SetClearColor({ 0, 0, 0, 0 });
	shadow_map_rt_->SetRenderTarget();

	DX11APP->GetDeviceContext()->VSSetShader(nullptr, nullptr, 0);
	DX11APP->GetDeviceContext()->GSSetShader(nullptr, nullptr, 0);
	DX11APP->GetDeviceContext()->PSSetShader(nullptr, nullptr, 0);


	DX11APP->GetDeviceContext()->VSSetShader(vertex_shader_->Get(), nullptr, 0);
	DX11APP->GetDeviceContext()->PSSetShader(pixel_shader_->Get(), nullptr, 0);
	DX11APP->GetDeviceContext()->IASetInputLayout(vertex_shader_->InputLayout());

	for (auto& mesh : static_mesh_level->GetLevelMesh()->meshes)
	{
		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		DX11APP->GetDeviceContext()->Draw(mesh.vertices.size(), 0);
	}

	RENDER_TARGET->ResettingRT();
}
