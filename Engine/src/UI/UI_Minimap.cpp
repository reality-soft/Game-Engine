#include "stdafx.h"
#include "UI_Minimap.h"
#include "SceneMgr.h"
#include "Character.h"
#include "RenderTargetMgr.h"
#include "Engine.h"
#include "UISystem.h"
#include "ResourceMgr.h"

using namespace reality;

void UI_Minimap::InitMinimap(string minimap_id)
{
	UIBase::Init();

	reg_ = &SCENE_MGR->GetRegistry();

	minimap_tex_id_ = minimap_id;
	auto texture = RESOURCE->UseResource<Texture>(minimap_tex_id_);

	minimap_rt = RENDER_TARGET->MakeRT("Minimap UI", texture->texture_desc.Width, texture->texture_desc.Height);
	minimap_rt->SetClearColor({ 0, 0, 0, 0 });
}

void UI_Minimap::UpdateThisUI()
{
	UIBase::UpdateThisUI();

	// 0. �̴ϸ� ����Ÿ�ٿ� �ִ� srv�� �׸��� ��� ����
	minimap_rt->SetRenderTarget();

	// 1. �̴ϸ� �ؽ��ĸ� ����Ÿ�� srv�� ������
	RenderMinimap();

	// 2. �÷��̾� ������ ������
	XMMATRIX player_world = SCENE_MGR->GetPlayer<Character>(0)->GetTranformMatrix();
	RenderPlayerIcon(player_world);

	// 3. ���� ������ ������ : TODO
	//RenderZombieIcon();

	// 4. ������ �� �ٽ� �� ���۷� �׸��� ��� ����
	DX11APP->SetBackBufferRTV();
}

void UI_Minimap::RenderThisUI()
{
	// �����ܵ��� �׷��� �ؽ��ĸ� �÷��̾� ��ġ�� ���缭 �ؽ��� ��ǥ�� �����ؼ� ������
	
	// TODO : �÷��̾� ��ġ�� �°� �ؽ��� ���� �� ���� ���� ������Ʈ

	// Set Constant Buffer :TODO-�̴ϸ���ġ�� �°� ����
	UISystem::SetCbData(XMMatrixTranspose(rect_transform_.world_matrix));

	// Set Topology
	DX11APP->GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set Vertex Buffer
	UINT stride = sizeof(UIVertex);
	UINT offset = 0;
	DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, render_data_.vertex_buffer.GetAddressOf(), &stride, &offset);

	// Set Index Buffer
	DX11APP->GetDeviceContext()->IASetIndexBuffer(render_data_.index_buffer.Get(), DXGI_FORMAT_R32_UINT, offset);

	// Get Shader From ResourceMgr
	VertexShader* vs = RESOURCE->UseResource<VertexShader>(render_data_.vs_id);
	PixelShader* ps = RESOURCE->UseResource<PixelShader>(render_data_.ps_id);

	if (vs == nullptr || ps == nullptr)
		return;

	// Set InputLayout
	DX11APP->GetDeviceContext()->IASetInputLayout(vs->InputLayout());

	// Set VS
	DX11APP->GetDeviceContext()->VSSetShader(vs->Get(), 0, 0);

	// Set PS
	DX11APP->GetDeviceContext()->PSSetShader(ps->Get(), 0, 0);

	// Reset GS
	DX11APP->GetDeviceContext()->GSSetShader(nullptr, 0, 0);

	// Set Texture
	DX11APP->GetDeviceContext()->PSSetShaderResources(0, 1, minimap_rt->render_target_view_srv_.GetAddressOf());

	// Draw Indexed
	DX11APP->GetDeviceContext()->DrawIndexed(render_data_.index_list.size(), 0, 0);
}

void UI_Minimap::RenderMinimap()
{
	// Set Constant Buffer
	XMVECTOR pos = XMVectorSet(0.5f, 0.5f, 0.0f, 0.0f);
	XMMATRIX S, R, T;
	S = XMMatrixIdentity();
	R = XMMatrixIdentity();
	T = XMMatrixTranslationFromVector(pos);
	XMMATRIX minimap_world = S * R * T;

	UISystem::SetCbData(XMMatrixTranspose(minimap_world));

	// Set Texture
	Texture* texture = RESOURCE->UseResource<Texture>(minimap_tex_id_);
	DX11APP->GetDeviceContext()->PSSetShaderResources(0, 1, texture->srv.GetAddressOf());

	RenderExceptTextureSet();
}

void UI_Minimap::RenderPlayerIcon(XMMATRIX world)
{
	// TODO : world ��ǥ�� �̴ϸ� ������ ���� ����
	XMVECTOR world_scale, world_rot, world_pos;
	XMMatrixDecompose(&world_scale, &world_rot, &world_pos, world);

	XMMATRIX S, R, T;

	XMVECTOR scale = XMVectorSet(0.032f, 0.032f, 0.0f, 1.0f);
	XMVECTOR pos = ConvertWorldToScreenXZ(world_pos);
	S = XMMatrixScalingFromVector(scale);
	R = XMMatrixIdentity();
	T = XMMatrixTranslationFromVector(pos);
	XMMATRIX minimap_world = S * R * T;

	// Set Constant Buffer
	UISystem::SetCbData(XMMatrixTranspose(minimap_world));

	// Set Texture
	Texture* texture = RESOURCE->UseResource<Texture>("T_Player_Icon.png");
	DX11APP->GetDeviceContext()->PSSetShaderResources(0, 1, texture->srv.GetAddressOf());

	RenderExceptTextureSet();
}

void UI_Minimap::RenderZombieIcon(XMMATRIX world)
{
	// TODO : world ��ǥ�� �̴ϸ� ������ ���� ����
	XMMATRIX minimap_world = world;

	// Set Constant Buffer
	UISystem::SetCbData(XMMatrixTranspose(minimap_world));

	// Set Texture
	Texture* texture = RESOURCE->UseResource<Texture>("T_Zombie_Icon.png");
	DX11APP->GetDeviceContext()->PSSetShaderResources(0, 1, texture->srv.GetAddressOf());

	RenderExceptTextureSet();
}

void UI_Minimap::RenderExceptTextureSet()
{
	// Set Topology
	DX11APP->GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set Vertex Buffer
	UINT stride = sizeof(UIVertex);
	UINT offset = 0;
	DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, render_data_.vertex_buffer.GetAddressOf(), &stride, &offset);

	// Set Index Buffer
	DX11APP->GetDeviceContext()->IASetIndexBuffer(render_data_.index_buffer.Get(), DXGI_FORMAT_R32_UINT, offset);

	// Get Shader From ResourceMgr
	VertexShader* vs = RESOURCE->UseResource<VertexShader>(render_data_.vs_id);
	PixelShader* ps = RESOURCE->UseResource<PixelShader>(render_data_.ps_id);

	if (vs == nullptr || ps == nullptr)
		return;

	// Set InputLayout
	DX11APP->GetDeviceContext()->IASetInputLayout(vs->InputLayout());

	// Set VS
	DX11APP->GetDeviceContext()->VSSetShader(vs->Get(), 0, 0);

	// Set PS
	DX11APP->GetDeviceContext()->PSSetShader(ps->Get(), 0, 0);

	// Reset GS
	DX11APP->GetDeviceContext()->GSSetShader(nullptr, 0, 0);

	// Draw Indexed
	DX11APP->GetDeviceContext()->DrawIndexed(render_data_.index_list.size(), 0, 0);
}

XMVECTOR UI_Minimap::ConvertWorldToScreenXZ(XMVECTOR world)
{
	
	// X
	float world_x = world.m128_f32[0];
	float screen_x = world_x / 15000.0f + 0.5f;

	// Z
	float world_z = world.m128_f32[2];
	float screen_z = screen_z = 0.5f - world_z / 15000.0f;


	XMVECTOR convert_vector = XMVectorSet(screen_x, screen_z, 0, 1.0f);

	return convert_vector;
}
