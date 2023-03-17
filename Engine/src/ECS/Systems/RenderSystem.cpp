#include "stdafx.h"
#include "RenderSystem.h"
#include "ResourceMgr.h"
#include "TimeMgr.h"
#include "DataMgr.h"
#include "DataTypes.h"
#include "DXStates.h"

using namespace reality;

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


void reality::RenderSystem::OnCreate(entt::registry& reg)
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

	// Create Effect Data
	CreateEffectCB();
	CreateEffectBuffer();
}

void RenderSystem::OnUpdate(entt::registry& reg)
{
	DX11APP->GetDeviceContext()->GSSetShader(nullptr, nullptr, 0);

	auto view_stm = reg.view<C_StaticMesh>();
	auto view_skm = reg.view<C_SkeletalMesh>();
	auto view_bb = reg.view<C_BoundingBox>();

	for (auto ent : view_stm)
	{
		auto* static_mesh = reg.try_get<C_StaticMesh>(ent);
		RenderStaticMesh(static_mesh);
	}

	for (auto ent : view_skm)
	{
		auto* skeletal_mesh = reg.try_get<C_SkeletalMesh>(ent);
		auto* animation = reg.try_get<C_Animation>(ent);
		if (skeletal_mesh == nullptr || animation == nullptr) {
			continue;
		}
		RenderSkeletalMesh(skeletal_mesh, animation);
	}

	DX11APP->GetDeviceContext()->RSSetState(DXStates::rs_wireframe_cull_none());
	for (auto ent : view_bb)
	{
		auto* bounding_box = reg.try_get<C_BoundingBox>(ent);
		RenderBoundingBox(bounding_box);
	}
	DX11APP->GetDeviceContext()->RSSetState(DXStates::rs_solid_cull_none());

	// BoxShape Render
	RenderBoxShape(reg);
	

	// Emitter Render
	RenderEffects(reg);
}

void RenderSystem::SetCbTransform(const C_Transform* const transform)
{
	cb_transform.data.world_matrix = XMMatrixTranspose(transform->local * transform->world);

	device_context->UpdateSubresource(cb_transform.buffer.Get(), 0, nullptr, &cb_transform.data, 0, 0);
	device_context->VSSetConstantBuffers(1, 1, cb_transform.buffer.GetAddressOf());
}

void RenderSystem::PlayAnimation(const Skeleton& skeleton, const vector<OutAnimData>& res_animation)
{
	static float keyframe = res_animation.begin()->start_frame;

	if (keyframe >= res_animation.begin()->end_frame)
		keyframe = res_animation.begin()->start_frame;

	for (auto bp : skeleton.bind_pose_matrices)
	{
		XMMATRIX anim_matrix = bp.second * res_animation.begin()->animations.find(bp.first)->second[keyframe];
		cb_skeleton.data.mat_skeleton[bp.first] = XMMatrixTranspose(anim_matrix);
	}

	keyframe += 60.f / TM_FPS;

	device_context->UpdateSubresource(cb_skeleton.buffer.Get(), 0, nullptr, &cb_skeleton.data, 0, 0);
	device_context->VSSetConstantBuffers(2, 1, cb_skeleton.buffer.GetAddressOf());
}

void RenderSystem::RenderStaticMesh(const C_StaticMesh* const static_mesh_component)
{
	StaticMesh* static_mesh = RESOURCE->UseResource<StaticMesh>(static_mesh_component->static_mesh_id);
	VertexShader* shader = RESOURCE->UseResource<VertexShader>(static_mesh_component->vertex_shader_id);

	SetCbTransform(static_mesh_component);

	for (auto single_mesh : static_mesh->meshes)
	{
		Material* material = RESOURCE->UseResource<Material>(single_mesh.mesh_name + ".mat");
		if (material)
			material->Set();

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		device_context->IASetVertexBuffers(0, 1, single_mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		device_context->IASetIndexBuffer(single_mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		device_context->IASetInputLayout(shader->InputLayout());
		device_context->VSSetShader(shader->Get(), 0, 0);

		device_context->DrawIndexed(single_mesh.indices.size(), 0, 0);
	}

}

void RenderSystem::RenderSkeletalMesh(const C_SkeletalMesh* const skeletal_mesh_components, const C_Animation* const animation_component)
{
	SkeletalMesh* skeletal_mesh = RESOURCE->UseResource<SkeletalMesh>(skeletal_mesh_components->skeletal_mesh_id);
	VertexShader* shader = RESOURCE->UseResource<VertexShader>(skeletal_mesh_components->vertex_shader_id);
	vector<OutAnimData>* res_animation = RESOURCE->UseResource<vector<OutAnimData>>(animation_component->anim_id);
	if (res_animation != nullptr) {
		PlayAnimation(skeletal_mesh->skeleton, *res_animation);
	}

	SetCbTransform(skeletal_mesh_components);

	for (auto& single_mesh : skeletal_mesh->meshes)
	{
		Material* material = RESOURCE->UseResource<Material>(single_mesh.mesh_name + ".mat");
		if (material)
			material->Set();

		UINT stride = sizeof(SkinnedVertex);
		UINT offset = 0;
		device_context->IASetVertexBuffers(0, 1, single_mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		device_context->IASetIndexBuffer(single_mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		
		device_context->IASetInputLayout(shader->InputLayout());
		device_context->VSSetShader(shader->Get(), 0, 0);

		device_context->DrawIndexed(single_mesh.indices.size(), 0, 0);
	}
}

void RenderSystem::CreateEffectCB()
{
	HRESULT hr;
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA subdata;

	// Init Effect Buffer
	{
		ZeroMemory(&cb_effect.data, sizeof(CbEffect::Data));

		ZeroMemory(&desc, sizeof(desc));
		ZeroMemory(&subdata, sizeof(subdata));

		cb_effect.data.world = XMMatrixIdentity();

		desc.ByteWidth = sizeof(CbEffect::Data);

		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		subdata.pSysMem = &cb_effect.data;

		hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, cb_effect.buffer.GetAddressOf());
	}

	// Init Emitter Buffer
	{
		ZeroMemory(&cb_emitter.data, sizeof(CbEmitter::Data));

		ZeroMemory(&desc, sizeof(desc));
		ZeroMemory(&subdata, sizeof(subdata));

		cb_emitter.data.world = XMMatrixIdentity();
		ZeroMemory(&cb_emitter.data.value, sizeof(XMINT4) * 4);
		ZeroMemory(&cb_emitter.data.value2, sizeof(XMFLOAT4) * 4);

		desc.ByteWidth = sizeof(CbEmitter::Data);

		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		subdata.pSysMem = &cb_emitter.data;

		hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, cb_emitter.buffer.GetAddressOf());
	}
	
	// Init Particle Buffer
	{
		ZeroMemory(&cb_particle_.data, sizeof(CbParticle::Data));

		cb_particle_.data.values.x = 0.0f;
		cb_particle_.data.values.y = 0.0f;
		cb_particle_.data.values.z = 0.0f;
		cb_particle_.data.values.w = 0.0f;

		cb_particle_.data.color.x = 1.0f;
		cb_particle_.data.color.y = 1.0f;
		cb_particle_.data.color.z = 1.0f;
		cb_particle_.data.color.w = 1.0f;

		cb_particle_.data.transform = XMMatrixIdentity();

		ZeroMemory(&desc, sizeof(desc));
		ZeroMemory(&subdata, sizeof(subdata));

		desc.ByteWidth = sizeof(CbParticle::Data);

		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		subdata.pSysMem = &cb_particle_.data;

		hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, cb_particle_.buffer.GetAddressOf());
	}
	
}

void RenderSystem::CreateEffectBuffer()
{
	effect_vertex_.p = { 0, 0, 0 };
	effect_vertex_.c = { 1.0f, 1.0f, 1.0f, 1.0f };
	effect_vertex_.t = { 0.5f, 0.5f };

	D3D11_BUFFER_DESC bufDesc;

	ZeroMemory(&bufDesc, sizeof(D3D11_BUFFER_DESC));

	bufDesc.ByteWidth = sizeof(EffectVertex);
	bufDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = 0;
	bufDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subResourse;

	ZeroMemory(&subResourse, sizeof(D3D11_SUBRESOURCE_DATA));

	subResourse.pSysMem = &effect_vertex_;
	subResourse.SysMemPitch;
	subResourse.SysMemSlicePitch;

	DX11APP->GetDevice()->CreateBuffer(&bufDesc, &subResourse, &vertex_buffer_);
}

void RenderSystem::RenderBoxShape(entt::registry& reg)
{
	auto view_box = reg.view<C_BoxShape, C_Transform>();
	for (auto ent : view_box)
	{
		auto& box = reg.get<C_BoxShape>(ent);
		SetCbTransform(&box);

		auto material = RESOURCE->UseResource<Material>(box.material_id);
		material->Set();

		VertexShader* shader = RESOURCE->UseResource<VertexShader>(box.vs_id);
		if (shader == nullptr)
			return;

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		device_context->IASetVertexBuffers(0, 1, box.vertex_buffer.GetAddressOf(), &stride, &offset);
		device_context->IASetIndexBuffer(box.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		device_context->IASetInputLayout(shader->InputLayout());
		device_context->VSSetShader(shader->Get(), 0, 0);

		device_context->DrawIndexed(box.index_list.size(), 0, 0);
	}
}

void RenderSystem::RenderBoundingBox(const C_BoundingBox* const box)
{
	VertexShader* shader = RESOURCE->UseResource<VertexShader>(box->vs_id);
	if (shader == nullptr)
		return;

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	SetCbTransform(box);

	device_context->IASetVertexBuffers(0, 1, box->vertex_buffer.GetAddressOf(), &stride, &offset);
	device_context->IASetIndexBuffer(box->index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	device_context->IASetInputLayout(shader->InputLayout());
	device_context->VSSetShader(shader->Get(), 0, 0);

	device_context->DrawIndexed(box->index_list.size(), 0, 0);
}

void RenderSystem::RenderEffects(entt::registry& reg)
{
	auto view_effect = reg.view<C_Effect>();
	for (auto ent : view_effect)
	{
		device_context->IASetIndexBuffer(nullptr, DXGI_FORMAT_R16_UINT, 0);
 		auto& effect_comp = reg.get<C_Effect>(ent);

		auto& effect = effect_comp.effect;

		SetEffectCB(effect, effect_comp.world);

		for (auto& pair : effect.emitters)
		{
			auto& emitter = pair.second;

			// Effect 설정
			SetEmitterCB(emitter);

			// Sprite 정보 설정
			Sprite* sprite = RESOURCE->UseResource<Sprite>(emitter.sprite_id);
			if (sprite == nullptr)
				return;

			// 이펙트의 쉐이더 및 머터리얼 설정
			SetShaderAndMaterial(emitter);

			// BS 및 DS 설정
			SetStates(emitter);

			// particle 설정
			for (auto& particle : emitter.particles)
			{
				if (!particle.enable)
					continue;

				if (sprite->type == TEX_SPRITE)
				{
					TextureSprite* tex_sprite = (TextureSprite*)sprite;
					Texture* texture = RESOURCE->UseResource<Texture>(tex_sprite->tex_id_list[particle.timer]);
					if (texture != nullptr)
						device_context->PSSetShaderResources(0, 1, texture->srv.GetAddressOf());
				}

				SetParticleCB(particle);

				// VertexBuffer 설정
				UINT stride = sizeof(EffectVertex);
				UINT offset = 0;
				device_context->IASetVertexBuffers(0, 1, vertex_buffer_.GetAddressOf(), &stride, &offset);
				device_context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
				device_context->Draw(1, 0);
			}
		}
	}

	// BS, DS 복구 작업
	device_context->OMSetBlendState(DXStates::bs_default(), nullptr, -1);
	device_context->OMSetDepthStencilState(DXStates::ds_defalut(), 0xff);
	device_context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	device_context->GSSetShader(nullptr, 0, 0);
}

void RenderSystem::SetEffectCB(Effect& effect, XMMATRIX& world)
{
	cb_effect.data.world = XMMatrixTranspose(world);

	device_context->UpdateSubresource(cb_effect.buffer.Get(), 0, nullptr, &cb_effect.data, 0, 0);
	device_context->GSSetConstantBuffers(1, 1, cb_effect.buffer.GetAddressOf());
}

void RenderSystem::SetEmitterCB(Emitter& emitter)
{
	// Sprite 정보 입력
	Sprite* sprite = RESOURCE->UseResource<Sprite>(emitter.sprite_id);

	switch (sprite->type)
	{
		// UV
	case UV_SPRITE:
	{
		// type
		cb_emitter.data.value.x = 0;


		UVSprite* uv_sprite = (UVSprite*)sprite;

		Texture* texture = RESOURCE->UseResource<Texture>(uv_sprite->tex_id);
		if (texture != nullptr)
			device_context->PSSetShaderResources(0, 1, texture->srv.GetAddressOf());

		cb_emitter.data.value.z = uv_sprite->uv_list.size();

		for (int i = 0; i < uv_sprite->uv_list.size(); i++)
		{
			cb_emitter.data.value2[i].x = (float)uv_sprite->uv_list[i].first.x / (float)texture->texture_desc.Width;
			cb_emitter.data.value2[i].y = (float)uv_sprite->uv_list[i].first.y / (float)texture->texture_desc.Height;
			cb_emitter.data.value2[i].z = (float)uv_sprite->uv_list[i].second.x / (float)texture->texture_desc.Width;
			cb_emitter.data.value2[i].w = (float)uv_sprite->uv_list[i].second.y / (float)texture->texture_desc.Height;
		}
	}break;
	// Tex
	case TEX_SPRITE:
	{
		// type
		cb_emitter.data.value.x = 1;


	}break;
	}

	device_context->UpdateSubresource(cb_emitter.buffer.Get(), 0, nullptr, &cb_emitter.data, 0, 0);
	device_context->GSSetConstantBuffers(2, 1, cb_emitter.buffer.GetAddressOf());
}

void RenderSystem::SetShaderAndMaterial(Emitter& emitter)
{
	// 쉐이더 및 머터리얼 적용
	VertexShader* vs = RESOURCE->UseResource<VertexShader>(emitter.vs_id);
	if (vs)
	{
		device_context->IASetInputLayout(vs->InputLayout());
		device_context->VSSetShader(vs->Get(), 0, 0);
	}

	GeometryShader* gs = RESOURCE->UseResource<GeometryShader>(emitter.geo_id);
	if (gs)
		device_context->GSSetShader(gs->GetDefaultGS(), 0, 0);

	Material* material = RESOURCE->UseResource<Material>(emitter.mat_id);

	if (material)
		material->Set();
}

void RenderSystem::SetStates(Emitter& emitter)
{
	// BS 설정
	switch (emitter.bs_state)
	{
	case DEFAULT_BS:
		device_context->OMSetBlendState(DXStates::bs_default(), nullptr, -1);
		break;
	case NO_BLEND:
		device_context->OMSetBlendState(nullptr, nullptr, -1);
		break;
	case ALPHA_BLEND:
		device_context->OMSetBlendState(DXStates::bs_default(), nullptr, -1);
		break;
	case DUALSOURCE_BLEND:
		device_context->OMSetBlendState(DXStates::bs_dual_source_blend(), nullptr, -1);
		break;
	case HIGHER_RGB:
		device_context->OMSetBlendState(DXStates::bs_blend_higher_rgb(), nullptr, -1);
		break;

	}

	// DS 설정
	switch (emitter.ds_state)
	{
	case DEFAULT_NONE:
		device_context->OMSetDepthStencilState(DXStates::ds_defalut(), 0xff);
		break;
	case DEPTH_COMP_NOWRITE:
		device_context->OMSetDepthStencilState(DXStates::ds_depth_enable_no_write(), 0xff);
		break;
	case DEPTH_COMP_WRITE:
		device_context->OMSetDepthStencilState(DXStates::ds_defalut(), 0xff);
		break;
	}
}

void RenderSystem::SetParticleCB(Particle& particle)
{
	cb_particle_.data.color		= particle.color;
	cb_particle_.data.values.x	= particle.timer;
	cb_particle_.data.values.y	= particle.frame_ratio;

	//particle.rotation.z = XMConvertToRadians(particle.rotation.z);

	XMMATRIX s = XMMatrixScalingFromVector(XMLoadFloat3(&particle.scale));
	XMVECTOR rot_vec = { XMConvertToRadians(particle.rotation.x), XMConvertToRadians(particle.rotation.y), XMConvertToRadians(particle.rotation.z) };
	auto q = XMQuaternionRotationRollPitchYawFromVector(rot_vec);
	//auto q = XMQuaternionRotationRollPitchYaw(particle.rotation.x, particle.rotation.y, particle.rotation.z);
	XMMATRIX r = XMMatrixRotationQuaternion(q);
	//XMMATRIX r = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&particle.rotation));
	XMMATRIX t = XMMatrixTranslationFromVector(XMLoadFloat3(&particle.position));
	XMMATRIX sr = XMMatrixMultiply(s, r);
	XMMATRIX srt = XMMatrixMultiply(sr, t);

	cb_particle_.data.transform = XMMatrixTranspose(srt);

	device_context->UpdateSubresource(cb_particle_.buffer.Get(), 0, nullptr, &cb_particle_.data, 0, 0);
	device_context->GSSetConstantBuffers(3, 1, cb_particle_.buffer.GetAddressOf());
}

