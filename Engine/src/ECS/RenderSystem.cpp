#include "stdafx.h"
#include "RenderSystem.h"
#include "ResourceMgr.h"
#include "TimeMgr.h"
#include "DataMgr.h"
using namespace KGCA41B;

RenderSystem::RenderSystem()
{
	device = DX11APP->GetDevice();
	device_context = DX11APP->GetDeviceContext();
}

RenderSystem::~RenderSystem()
{
	device = nullptr;
	device_context = nullptr;
}

void KGCA41B::RenderSystem::OnCreate(entt::registry& reg)
{
	HRESULT hr;

	// Init Transform Buffer
	cb_transform.data.world_matrix = XMMatrixIdentity();
	
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA subdata;

	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.ByteWidth = sizeof(CbTransform::Data);

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	subdata.pSysMem = &cb_transform.data;

	hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, cb_transform.buffer.GetAddressOf());

	// Init SkeletonBuffer
	for (int i = 0; i < 255; ++i)
		cb_skeleton.data.mat_skeleton[i] = XMMatrixIdentity();

	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.ByteWidth = sizeof(CbSkeleton::Data);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	subdata.pSysMem = &cb_skeleton.data;

	hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, cb_skeleton.buffer.GetAddressOf());
}

void RenderSystem::OnUpdate(entt::registry& reg)
{
	auto view_stm = reg.view<Material, StaticMesh, Transform>();
	auto view_skm = reg.view<Material, SkeletalMesh, Transform>();

	for (auto ent : view_stm)
	{
		auto& transform = reg.get<Transform>(ent);
		SetCbTransform(transform);

		auto& material = reg.get<Material>(ent);
		SetMaterial(material);

		auto& static_mesh = reg.get<StaticMesh>(ent);
		RenderStaticMesh(static_mesh);
	}

	for (auto ent : view_skm)
	{
		auto [skeleton, animation] = reg.get<Skeleton, Animation>(ent);
		PlayAnimation(skeleton, animation);

		auto& transform = reg.get<Transform>(ent);
		SetCbTransform(transform);

		auto& material = reg.get<Material>(ent);
		SetMaterial(material);

		auto& skeletal_mesh = reg.get<SkeletalMesh>(ent);
		RenderSkeletalMesh(skeletal_mesh);
	}

	RenderEffects(reg);

}

void RenderSystem::SetMaterial(Material& material)
{
	PixelShader* shader = RESOURCE->UseResource<PixelShader>(material.shader_id);

	UINT slot = 0;
	for (auto tex_id : material.texture_id)
	{
		Texture* texture = RESOURCE->UseResource<Texture>(tex_id);
		if (texture != nullptr)
		{
			device_context->PSSetShaderResources(slot++, 1, texture->srv.GetAddressOf());
		}
	}	

	device_context->PSSetShader(shader->Get(), 0, 0);		
}

void RenderSystem::SetCbTransform(Transform& transform)
{
	cb_transform.data.world_matrix = XMMatrixTranspose(transform.world);

	device_context->UpdateSubresource(cb_transform.buffer.Get(), 0, nullptr, &cb_transform.data, 0, 0);
	device_context->VSSetConstantBuffers(0, 1, cb_transform.buffer.GetAddressOf());
}

void RenderSystem::PlayAnimation(Skeleton& skeleton, Animation& animation)
{
	map<UINT, XMMATRIX>* res_skeleton = RESOURCE->UseResource<map<UINT, XMMATRIX>>(skeleton.skeleton_id);
	vector<OutAnimData>* res_animation = RESOURCE->UseResource<vector<OutAnimData>>(animation.anim_id);

	static float keyframe = res_animation->begin()->start_frame;

	if (keyframe >= res_animation->begin()->end_frame)
		keyframe = res_animation->begin()->start_frame;

	for (auto bp : *res_skeleton)
	{
		XMMATRIX anim_matrix = bp.second * res_animation->begin()->animations.find(bp.first)->second[keyframe];
		cb_skeleton.data.mat_skeleton[bp.first] = XMMatrixTranspose(anim_matrix);
	}

	keyframe += 60.f / TM_FPS;

	device_context->UpdateSubresource(cb_skeleton.buffer.Get(), 0, nullptr, &cb_skeleton.data, 0, 0);
	device_context->VSSetConstantBuffers(2, 1, cb_skeleton.buffer.GetAddressOf());
}

void RenderSystem::RenderStaticMesh(StaticMesh& static_mesh)
{
	vector<SingleMesh<Vertex>>* mesh_list = RESOURCE->UseResource<vector<SingleMesh<Vertex>>>(static_mesh.mesh_id);
	VertexShader* shader = RESOURCE->UseResource<VertexShader>(static_mesh.shader_id);

	for (auto single_mesh : *mesh_list)
	{
		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		device_context->IASetVertexBuffers(0, 1, single_mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		device_context->IASetIndexBuffer(single_mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		device_context->IASetInputLayout(shader->InputLayoyt());
		device_context->VSSetShader(shader->Get(), 0, 0);

		device_context->DrawIndexed(single_mesh.indices.size(), 0, 0);
	}
}

void RenderSystem::RenderSkeletalMesh(SkeletalMesh& skeletal_mesh)
{
	vector<SingleMesh<SkinnedVertex>>* mesh_list = RESOURCE->UseResource<vector<SingleMesh<SkinnedVertex>>>(skeletal_mesh.mesh_id);
	VertexShader* shader = RESOURCE->UseResource<VertexShader>(skeletal_mesh.shader_id);

	for (auto single_mesh : *mesh_list)
	{
		UINT stride = sizeof(SkinnedVertex);
		UINT offset = 0;
		device_context->IASetVertexBuffers(0, 1, single_mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		device_context->IASetIndexBuffer(single_mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		
		device_context->IASetInputLayout(shader->InputLayoyt());
		device_context->VSSetShader(shader->Get(), 0, 0);

		device_context->DrawIndexed(single_mesh.indices.size(), 0, 0);
	}
}

void KGCA41B::RenderSystem::RenderEffects(entt::registry& reg)
{
	auto view_uv_sprite = reg.view<UVSprite, Transform>();
	auto view_tex_sprite = reg.view<TextureSprite, Transform>();
	auto view_particles = reg.view<Particles, Transform>();

	for (auto ent : view_uv_sprite)
	{
		auto& transform = reg.get<Transform>(ent);
		SetCbTransform(transform);

		auto& uv_sprite = reg.get<UVSprite>(ent);

		if (!uv_sprite.enabled_)
			continue;


		Texture* texture = RESOURCE->UseResource<Texture>(uv_sprite.tex_id);

		// uv °ª ¼³Á¤
		auto uv_value = uv_sprite.uv_list[min((int)uv_sprite.cur_frame - 1, (int)uv_sprite.uv_list.size() - 1)];

		float tex_width = (float)texture->texture_desc.Width;
		float tex_height = (float)texture->texture_desc.Height;

		uv_sprite.vertex_list[0].t.x = uv_value.first.x / tex_width;
		uv_sprite.vertex_list[0].t.y = uv_value.first.y / tex_height;

		uv_sprite.vertex_list[1].t.x = uv_value.second.x / tex_width;
		uv_sprite.vertex_list[1].t.y = uv_value.first.y / tex_height;

		uv_sprite.vertex_list[2].t.x = uv_value.first.x / tex_width;
		uv_sprite.vertex_list[2].t.y = uv_value.second.y / tex_height;

		uv_sprite.vertex_list[3].t.x = uv_value.second.x / tex_width;
		uv_sprite.vertex_list[3].t.y = uv_value.second.y / tex_height;

		VertexShader* vs =		RESOURCE->UseResource<VertexShader>(uv_sprite.vs_id);
		PixelShader* ps =		RESOURCE->UseResource<PixelShader>(uv_sprite.ps_id);

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		if (uv_sprite.vertex_buffer != nullptr)
		{
			device_context->UpdateSubresource(uv_sprite.vertex_buffer.Get(), 0, nullptr, &uv_sprite.vertex_list.at(0), 0, 0);
			device_context->IASetVertexBuffers(0, 1, uv_sprite.vertex_buffer.GetAddressOf(), &stride, &offset);
		}

		if (uv_sprite.index_buffer != nullptr)
			device_context->IASetIndexBuffer(uv_sprite.index_buffer.Get(), DXGI_FORMAT_R32_UINT, offset);

		if (vs != nullptr)
			device_context->IASetInputLayout(vs->InputLayoyt());

		if (vs != nullptr)
			device_context->VSSetShader(vs->Get(), 0, 0);
		if (ps != nullptr)
			device_context->PSSetShader(ps->Get(), 0, 0);

		if (texture != nullptr)
			device_context->PSSetShaderResources(0, 1, texture->srv.GetAddressOf());

		device_context->DrawIndexed(uv_sprite.index_list.size(), 0, 0);
	}

	for (auto ent : view_tex_sprite)
	{
		auto& transform = reg.get<Transform>(ent);
		SetCbTransform(transform);

		auto& tex_sprite = reg.get<TextureSprite>(ent);

		if (!tex_sprite.enabled_)
			continue;

		VertexShader* vs = RESOURCE->UseResource<VertexShader>(tex_sprite.vs_id);
		PixelShader* ps = RESOURCE->UseResource<PixelShader>(tex_sprite.ps_id);

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		if (tex_sprite.vertex_buffer != nullptr)
			device_context->IASetVertexBuffers(0, 1, tex_sprite.vertex_buffer.GetAddressOf(), &stride, &offset);

		if (tex_sprite.index_buffer != nullptr)
			device_context->IASetIndexBuffer(tex_sprite.index_buffer.Get(), DXGI_FORMAT_R32_UINT, offset);

		if (vs != nullptr)
			device_context->IASetInputLayout(vs->InputLayoyt());

		if (vs != nullptr)
			device_context->VSSetShader(vs->Get(), 0, 0);
		if (ps != nullptr)
			device_context->PSSetShader(ps->Get(), 0, 0);
		
		Texture* texture = RESOURCE->UseResource<Texture>(tex_sprite.tex_id_list[min((int)tex_sprite.cur_frame - 1, (int)tex_sprite.tex_id_list.size() - 1)]);

		if (texture != nullptr)
			device_context->PSSetShaderResources(0, 1, texture->srv.GetAddressOf());

		device_context->DrawIndexed(tex_sprite.index_list.size(), 0, 0);
	}

	for (auto ent : view_particles)
	{
		auto& transform = reg.get<Transform>(ent);
		SetCbTransform(transform);

		auto& particles = reg.get<Particles>(ent);

		if (!particles.enabled_)
			return;

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		VertexShader* vs = RESOURCE->UseResource<VertexShader>(particles.vs_id);
		GeometryShader* gs = RESOURCE->UseResource<GeometryShader>(particles.geo_id);
		PixelShader* ps = RESOURCE->UseResource<PixelShader>(particles.ps_id);

		device_context->IASetVertexBuffers(0, 1, particles.vertex_buffer.GetAddressOf(), &stride, &offset);

		if (vs != nullptr)
			device_context->IASetInputLayout(vs->InputLayoyt());

		if (vs != nullptr)
			device_context->VSSetShader(vs->Get(), 0, 0);
		if (gs != nullptr)
			device_context->GSSetShader(gs->Get(), 0, 0);
		if (ps != nullptr)
			device_context->PSSetShader(ps->Get(), 0, 0);

		Texture* texture = RESOURCE->UseResource<Texture>(particles.tex_id_list[0]);

		if (texture != nullptr)
			device_context->PSSetShaderResources(0, 1, texture->srv.GetAddressOf());

		device_context->Draw(particles.vertex_list.size(), 0);
	}
}

