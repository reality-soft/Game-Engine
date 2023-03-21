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

	// 0. 미니맵 랜더타겟에 있는 srv로 그리기 대상 설정
	minimap_rt->SetRenderTarget();

	// 1. 미니맵 텍스쳐를 랜더타겟 srv에 랜더링
	RenderMinimap();

	// 2. 플레이어 아이콘 랜더링
	XMMATRIX player_world = SCENE_MGR->GetPlayer<Character>(0)->GetTranformMatrix();
	RenderPlayerIcon(player_world);

	// 3. 좀비 아이콘 랜더링 : TODO
	//RenderZombieIcon();

	// 4. 랜더링 후 다시 백 버퍼로 그리기 대상 설정
	DX11APP->SetBackBufferRTV();
}

void UI_Minimap::RenderThisUI()
{
	// 아이콘들이 그려진 텍스쳐를 플레이어 위치에 맞춰서 텍스쳐 좌표를 수정해서 랜더링
	
	// TODO : 플레이어 위치에 맞게 텍스쳐 수정 후 정점 버퍼 업데이트

	// Set Constant Buffer :TODO-미니맵위치에 맞게 수정
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
	// TODO : world 좌표를 미니맵 비율에 따라 조정
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
	// TODO : world 좌표를 미니맵 비율에 따라 조정
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
