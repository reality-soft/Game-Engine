#include "stdafx.h"
#include "RenderSystem.h"
#include "ResourceMgr.h"
#include "TimeMgr.h"
#include "DataTableMgr.h"
#include "DXStates.h"

using namespace reality;

RenderSystem::RenderSystem()
{
	device_ = DX11APP->GetDevice();
	device_context_ = DX11APP->GetDeviceContext();
}

RenderSystem::~RenderSystem()
{
	device_ = nullptr;
	device_context_ = nullptr;
}


void reality::RenderSystem::OnCreate(entt::registry& reg)
{
	HRESULT hr;
	
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA subdata;

	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.ByteWidth = sizeof(CbTransform::Data);

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	subdata.pSysMem = &cb_transform_.data;

	hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, cb_transform_.buffer.GetAddressOf());

	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.ByteWidth = sizeof(CbStaticMesh::Data);

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	subdata.pSysMem = &cb_static_mesh_.data;

	hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, cb_static_mesh_.buffer.GetAddressOf());

	for (int i = 0; i < 128; ++i) {
		cb_skeletal_mesh_.data.bind_pose[i] = XMMatrixIdentity();
		cb_skeletal_mesh_.data.animation[i] = XMMatrixIdentity();
		cb_skeletal_mesh_.data.slot_animation[i] = XMMatrixIdentity();
	}

	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.ByteWidth = sizeof(CbSkeletalMesh::Data);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	subdata.pSysMem = &cb_skeletal_mesh_.data;

	hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, cb_skeletal_mesh_.buffer.GetAddressOf());

	// Create Effect Data
	CreateEffectCB();
	CreateEffectBuffer();
}

void RenderSystem::OnUpdate(entt::registry& reg)
{
	DX11APP->GetDeviceContext()->GSSetShader(nullptr, nullptr, 0);

	auto view_stm = reg.view<C_StaticMesh>();
	auto view_skm = reg.view<C_SkeletalMesh>();

	for (auto ent : view_stm)
	{
		auto* static_mesh_component = reg.try_get<C_StaticMesh>(ent);
		C_Socket* socket_component = nullptr;
		if (static_mesh_component->socket_name != "") {
			socket_component = reg.try_get<C_Socket>(ent);
		}

		if (socket_component != nullptr) {
			C_Transform transform;
			transform.world = socket_component->world;
			transform.local = static_mesh_component->local;
			SetTransformCB(&transform, cb_static_mesh_.data.transform);
			auto socket_it = socket_component->sockets.find(static_mesh_component->socket_name);
			if (socket_it != socket_component->sockets.end()) {
				SetSocketCB(socket_it->second);
			}
		}
		else {
			SetTransformCB(static_mesh_component, cb_static_mesh_.data.transform);
		}

		RenderStaticMesh(static_mesh_component);
	}

	for (auto ent : view_skm)
	{
		auto* skeletal_mesh_component = reg.try_get<C_SkeletalMesh>(ent);
		auto* animation_component = reg.try_get<C_Animation>(ent);
		if (skeletal_mesh_component == nullptr || animation_component == nullptr) {
			continue;
		}
		SetTransformCB(skeletal_mesh_component, cb_skeletal_mesh_.data.transform);
		RenderSkeletalMesh(skeletal_mesh_component, animation_component);
	}

	DX11APP->GetDeviceContext()->RSSetState(DXStates::rs_solid_cull_none());

	// BoxShape Render
	RenderBoxShape(reg);
	
	// Emitter Render
	RenderEffects(reg);
}

void RenderSystem::PlayAnimation(const Skeleton& skeleton, C_Animation& animation_component)
{
	ZeroMemory(cb_skeletal_mesh_.data.slot_weights, sizeof(cb_skeletal_mesh_.data.slot_weights));

	for (const auto& bp : skeleton.bind_pose_matrices)
	{
		cb_skeletal_mesh_.data.bind_pose[bp.first] = XMMatrixTranspose(bp.second);
	}

	const AnimSlot& base_anim_slot = animation_component.anim_slots[0].second;
	ANIM_STATE base_anim_slot_state = base_anim_slot.GetCurAnimState();

	float base_time_weight = min(1.0f, base_anim_slot.anim_object_->GetCurAnimTime() / base_anim_slot.anim_object_->GetBlendTime());

	XMMATRIX base_cur_animation = XMMatrixIdentity();
	XMMATRIX base_prev_animation = XMMatrixIdentity();
	XMMATRIX base_animation = XMMatrixIdentity();

	OutAnimData* res_base_animation = nullptr;
	OutAnimData* res_base_prev_animation = nullptr;

	float base_cur_frame = base_anim_slot.anim_object_->GetCurFrame();
	float base_prev_last_frame = base_anim_slot.anim_object_->GetPrevAnimLastFrame();
	switch (base_anim_slot_state) {
	case ANIM_STATE::ANIM_STATE_NONE:
		for (const auto& bp : skeleton.bind_pose_matrices)
		{
			cb_skeletal_mesh_.data.animation[bp.first] = XMMatrixIdentity();
			cb_skeletal_mesh_.data.prev_animation[bp.first] = XMMatrixIdentity();
			cb_skeletal_mesh_.data.bind_pose[bp.first] = XMMatrixIdentity();
		}
		return;
	case ANIM_STATE::ANIM_STATE_CUR_ONLY:
		res_base_animation = RESOURCE->UseResource<OutAnimData>(base_anim_slot.anim_object_->GetCurAnimationId());
		for (const auto& bp : skeleton.bind_pose_matrices)
		{
			cb_skeletal_mesh_.data.prev_animation[bp.first] = XMMatrixTranspose(res_base_animation->animations[bp.first][base_cur_frame]);
			cb_skeletal_mesh_.data.animation[bp.first] = XMMatrixTranspose(res_base_animation->animations[bp.first][base_cur_frame]);
		}
		break;
	case ANIM_STATE::ANIM_STATE_CUR_PREV:
		res_base_prev_animation = RESOURCE->UseResource<OutAnimData>(base_anim_slot.anim_object_->GetPrevAnimationId());
		res_base_animation = RESOURCE->UseResource<OutAnimData>(base_anim_slot.anim_object_->GetCurAnimationId());

		for (const auto& bp : skeleton.bind_pose_matrices)
		{
			cb_skeletal_mesh_.data.prev_animation[bp.first] = XMMatrixTranspose(res_base_prev_animation->animations[bp.first][base_prev_last_frame]);
			cb_skeletal_mesh_.data.animation[bp.first] = XMMatrixTranspose(res_base_animation->animations[bp.first][base_cur_frame]);
		}
		break;
	}

	cb_skeletal_mesh_.data.base_time_weight = base_time_weight;

	int i = animation_component.anim_slots.size() - 1;

	for (i;i >= 1;i--) {
		AnimSlot& anim_slot = animation_component.anim_slots[i].second;
		ANIM_STATE slot_anim_state = anim_slot.GetCurAnimState();

		float slot_time_weight = min(1.0f, anim_slot.anim_object_->GetCurAnimTime() / anim_slot.anim_object_->GetBlendTime());

		XMMATRIX slot_cur_animation = XMMatrixIdentity();
		XMMATRIX slot_prev_animation = XMMatrixIdentity();
		XMMATRIX slot_animation = XMMatrixIdentity();

		OutAnimData* res_slot_animation = nullptr;
		OutAnimData* res_slot_prev_animation = nullptr;

		float slot_cur_frame = anim_slot.anim_object_->GetCurFrame();
		float slot_prev_last_frame = anim_slot.anim_object_->GetPrevAnimLastFrame();
		switch (slot_anim_state) {
		case ANIM_STATE::ANIM_STATE_NONE:
			continue;
		case ANIM_STATE::ANIM_STATE_PREV_ONLY:
			res_slot_prev_animation = RESOURCE->UseResource<OutAnimData>(anim_slot.anim_object_->GetPrevAnimationId());
			for (const auto& cur_pair : anim_slot.included_skeletons_) {
				int depth = cur_pair.first;

				for (const auto& bone_id : cur_pair.second) {
					cb_skeletal_mesh_.data.prev_slot_animation[bone_id] = XMMatrixTranspose(res_slot_prev_animation->animations[bone_id][slot_prev_last_frame]);
					cb_skeletal_mesh_.data.slot_animation[bone_id] = cb_skeletal_mesh_.data.animation[bone_id];

					cb_skeletal_mesh_.data.slot_weights[bone_id] = depth / anim_slot.range_;
				}
			}
			break;
		case ANIM_STATE::ANIM_STATE_CUR_ONLY:
			res_slot_animation = RESOURCE->UseResource<OutAnimData>(anim_slot.anim_object_->GetCurAnimationId());
			for (const auto& cur_pair : anim_slot.included_skeletons_) {
				int depth = cur_pair.first;

				for (const auto& bone_id : cur_pair.second) {
					cb_skeletal_mesh_.data.prev_slot_animation[bone_id] = cb_skeletal_mesh_.data.prev_animation[bone_id];
					cb_skeletal_mesh_.data.slot_animation[bone_id] = XMMatrixTranspose(res_slot_animation->animations[bone_id][slot_cur_frame]);

					cb_skeletal_mesh_.data.slot_weights[bone_id] = depth / anim_slot.range_;
				}
			}
			break;
		case ANIM_STATE::ANIM_STATE_CUR_PREV:
			res_slot_prev_animation = RESOURCE->UseResource<OutAnimData>(anim_slot.anim_object_->GetPrevAnimationId());
			res_slot_animation = RESOURCE->UseResource<OutAnimData>(anim_slot.anim_object_->GetCurAnimationId());
			for (const auto& cur_pair : anim_slot.included_skeletons_) {
				int depth = cur_pair.first;

				for (const auto& bone_id : cur_pair.second) {
					cb_skeletal_mesh_.data.prev_slot_animation[bone_id] = XMMatrixTranspose(res_slot_prev_animation->animations[bone_id][slot_prev_last_frame]);
					cb_skeletal_mesh_.data.slot_animation[bone_id] = XMMatrixTranspose(res_slot_animation->animations[bone_id][slot_cur_frame]);

					cb_skeletal_mesh_.data.slot_weights[bone_id] = depth / anim_slot.range_;
				}
			}
		}

		cb_skeletal_mesh_.data.slot_time_weight = slot_time_weight;
		break;
	}
}

void RenderSystem::RenderStaticMesh(const C_StaticMesh* const static_mesh_component)
{
	StaticMesh* static_mesh = RESOURCE->UseResource<StaticMesh>(static_mesh_component->static_mesh_id);
	VertexShader* shader = RESOURCE->UseResource<VertexShader>(static_mesh_component->vertex_shader_id);

	if (static_mesh == nullptr || shader == nullptr) {
		return;
	}

	device_context_->UpdateSubresource(cb_static_mesh_.buffer.Get(), 0, nullptr, &cb_static_mesh_.data, 0, 0);
	device_context_->VSSetConstantBuffers(1, 1, cb_static_mesh_.buffer.GetAddressOf());

	for (auto single_mesh : static_mesh->meshes)
	{
		Material* material = RESOURCE->UseResource<Material>(single_mesh.mesh_name + ".mat");
		if (material)
			material->Set();

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		device_context_->IASetVertexBuffers(0, 1, single_mesh.vertex_buffer.GetAddressOf(), &stride, &offset);

		device_context_->IASetInputLayout(shader->InputLayout());
		device_context_->VSSetShader(shader->Get(), 0, 0);

		ID3D11Buffer* index_buffer = single_mesh.index_buffer.Get();
		if (index_buffer == nullptr) {
			device_context_->Draw(single_mesh.vertices.size(), 0);
		}
		else {
			device_context_->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
			device_context_->DrawIndexed(single_mesh.indices.size(), 0, 0);
		}
	}
}

void RenderSystem::RenderSkeletalMesh(const C_SkeletalMesh* const skeletal_mesh_components, C_Animation* const animation_component)
{
	SkeletalMesh* skeletal_mesh = RESOURCE->UseResource<SkeletalMesh>(skeletal_mesh_components->skeletal_mesh_id);
	VertexShader* shader = RESOURCE->UseResource<VertexShader>(skeletal_mesh_components->vertex_shader_id);

	if (skeletal_mesh == nullptr || shader == nullptr) {
		return;
	}

	PlayAnimation(skeletal_mesh->skeleton, *animation_component);
	
	device_context_->UpdateSubresource(cb_skeletal_mesh_.buffer.Get(), 0, nullptr, &cb_skeletal_mesh_.data, 0, 0);
	device_context_->VSSetConstantBuffers(1, 1, cb_skeletal_mesh_.buffer.GetAddressOf());

	for (auto& single_mesh : skeletal_mesh->meshes)
	{
		Material* material = RESOURCE->UseResource<Material>(single_mesh.mesh_name + ".mat");
		if (material)
			material->Set();

		UINT stride = sizeof(SkinnedVertex);
		UINT offset = 0;

		device_context_->IASetVertexBuffers(0, 1, single_mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		
		device_context_->IASetInputLayout(shader->InputLayout());
		device_context_->VSSetShader(shader->Get(), 0, 0);
		
		ID3D11Buffer* index_buffer = single_mesh.index_buffer.Get();
		if (index_buffer == nullptr) {
			device_context_->Draw(single_mesh.vertices.size(), 0);
		}
		else {
			device_context_->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
			device_context_->DrawIndexed(single_mesh.indices.size(), 0, 0);
		}
	}
}

void RenderSystem::CreateEffectCB()
{
	HRESULT hr;
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA subdata;

	// Init Effect Buffer
	{
		ZeroMemory(&cb_effect_.data, sizeof(CbEffect::Data));

		ZeroMemory(&desc, sizeof(desc));
		ZeroMemory(&subdata, sizeof(subdata));

		cb_effect_.data.world = XMMatrixIdentity();

		desc.ByteWidth = sizeof(CbEffect::Data);

		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		subdata.pSysMem = &cb_effect_.data;
		subdata.pSysMem = &cb_effect_.data;

		hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, cb_effect_.buffer.GetAddressOf());
	}

	// Init Emitter Buffer
	{
		ZeroMemory(&cb_emitter_.data, sizeof(CbEmitter::Data));

		ZeroMemory(&desc, sizeof(desc));
		ZeroMemory(&subdata, sizeof(subdata));

		cb_emitter_.data.world = XMMatrixIdentity();
		ZeroMemory(&cb_emitter_.data.value, sizeof(XMINT4) * 4);
		ZeroMemory(&cb_emitter_.data.value2, sizeof(XMFLOAT4) * 4);

		desc.ByteWidth = sizeof(CbEmitter::Data);

		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		subdata.pSysMem = &cb_emitter_.data;

		hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, cb_emitter_.buffer.GetAddressOf());
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
		cb_particle_.data.transform_for_billboard = XMMatrixIdentity();

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

		SetTransformCB(&box, cb_transform_.data.transform);

		device_context_->UpdateSubresource(cb_transform_.buffer.Get(), 0, nullptr, &cb_transform_.data, 0, 0);
		device_context_->VSSetConstantBuffers(1, 1, cb_skeletal_mesh_.buffer.GetAddressOf());

		auto material = RESOURCE->UseResource<Material>(box.material_id);
		material->Set();

		VertexShader* shader = RESOURCE->UseResource<VertexShader>(box.vs_id);
		if (shader == nullptr)
			return;

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		device_context_->IASetVertexBuffers(0, 1, box.vertex_buffer.GetAddressOf(), &stride, &offset);
		device_context_->IASetIndexBuffer(box.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		device_context_->IASetInputLayout(shader->InputLayout());
		device_context_->VSSetShader(shader->Get(), 0, 0);

		device_context_->DrawIndexed(box.index_list.size(), 0, 0);
	}
}

void RenderSystem::RenderEffects(entt::registry& reg)
{
	auto view_effect = reg.view<C_Effect>();
	for (auto ent : view_effect)
	{
		device_context_->IASetIndexBuffer(nullptr, DXGI_FORMAT_R16_UINT, 0);
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
						device_context_->PSSetShaderResources(0, 1, texture->srv.GetAddressOf());
				}

				SetParticleCB(particle);

				// VertexBuffer 설정
				UINT stride = sizeof(EffectVertex);
				UINT offset = 0;
				device_context_->IASetVertexBuffers(0, 1, vertex_buffer_.GetAddressOf(), &stride, &offset);
				device_context_->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
				device_context_->Draw(1, 0);
			}
		}
	}

	// BS, DS 복구 작업
	device_context_->OMSetBlendState(DXStates::bs_default(), nullptr, -1);
	device_context_->OMSetDepthStencilState(DXStates::ds_defalut(), 0xff);
	device_context_->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	device_context_->GSSetShader(nullptr, 0, 0);
}

void reality::RenderSystem::SetTransformCB(const C_Transform* const transform_component, Transform& transform)
{
	transform.local_matrix = XMMatrixTranspose(transform_component->local);
	transform.world_matrix = XMMatrixTranspose(transform_component->world);
}

void reality::RenderSystem::SetSocketCB(const Socket& socket)
{
	cb_static_mesh_.data.socket_transform.owner_local = XMMatrixTranspose(socket.owner_local);
	cb_static_mesh_.data.socket_transform.local_offset = XMMatrixTranspose(socket.local_offset);
	cb_static_mesh_.data.socket_transform.animation_matrix = XMMatrixTranspose(socket.animation_matrix);
}

void RenderSystem::SetEffectCB(Effect& effect, XMMATRIX& world)
{
	cb_effect_.data.world = XMMatrixTranspose(world);

	device_context_->UpdateSubresource(cb_effect_.buffer.Get(), 0, nullptr, &cb_effect_.data, 0, 0);
	device_context_->GSSetConstantBuffers(1, 1, cb_effect_.buffer.GetAddressOf());
}

void RenderSystem::SetEmitterCB(Emitter& emitter)
{
	// Sprite 정보 입력
	Sprite* sprite = RESOURCE->UseResource<Sprite>(emitter.sprite_id);

	// gravity
	if (emitter.gravity_on_off)
		cb_emitter_.data.value.w = 1;
	else
		cb_emitter_.data.value.w = 0;

	switch (sprite->type)
	{
		// UV
	case UV_SPRITE:
	{
		// type
		cb_emitter_.data.value.x = 0;

		UVSprite* uv_sprite = (UVSprite*)sprite;

		Texture* texture = RESOURCE->UseResource<Texture>(uv_sprite->tex_id);
		if (texture != nullptr)
			device_context_->PSSetShaderResources(0, 1, texture->srv.GetAddressOf());

		cb_emitter_.data.value.z = uv_sprite->uv_list.size();

		for (int i = 0; i < uv_sprite->uv_list.size(); i++)
		{
			cb_emitter_.data.value2[i].x = (float)uv_sprite->uv_list[i].first.x / (float)texture->texture_desc.Width;
			cb_emitter_.data.value2[i].y = (float)uv_sprite->uv_list[i].first.y / (float)texture->texture_desc.Height;
			cb_emitter_.data.value2[i].z = (float)uv_sprite->uv_list[i].second.x / (float)texture->texture_desc.Width;
			cb_emitter_.data.value2[i].w = (float)uv_sprite->uv_list[i].second.y / (float)texture->texture_desc.Height;
		}
	}break;
	// Tex
	case TEX_SPRITE:
	{
		// type
		cb_emitter_.data.value.x = 1;


	}break;
	}

	device_context_->UpdateSubresource(cb_emitter_.buffer.Get(), 0, nullptr, &cb_emitter_.data, 0, 0);
	device_context_->GSSetConstantBuffers(2, 1, cb_emitter_.buffer.GetAddressOf());
}

void RenderSystem::SetShaderAndMaterial(Emitter& emitter)
{
	// 쉐이더 및 머터리얼 적용
	VertexShader* vs = RESOURCE->UseResource<VertexShader>(emitter.vs_id);
	if (vs)
	{
		device_context_->IASetInputLayout(vs->InputLayout());
		device_context_->VSSetShader(vs->Get(), 0, 0);
	}

	GeometryShader* gs = RESOURCE->UseResource<GeometryShader>(emitter.geo_id);
	if (gs)
		device_context_->GSSetShader(gs->GetDefaultGS(), 0, 0);

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
		device_context_->OMSetBlendState(DXStates::bs_default(), nullptr, -1);
		break;
	case NO_BLEND:
		device_context_->OMSetBlendState(nullptr, nullptr, -1);
		break;
	case ALPHA_BLEND:
		device_context_->OMSetBlendState(DXStates::bs_default(), nullptr, -1);
		break;
	case DUALSOURCE_BLEND:
		device_context_->OMSetBlendState(DXStates::bs_dual_source_blend(), nullptr, -1);
		break;
	case HIGHER_RGB:
		device_context_->OMSetBlendState(DXStates::bs_blend_higher_rgb(), nullptr, -1);
		break;

	}

	// DS 설정
	switch (emitter.ds_state)
	{
	case DEFAULT_NONE:
		device_context_->OMSetDepthStencilState(DXStates::ds_defalut(), 0xff);
		break;
	case DEPTH_COMP_NOWRITE:
		device_context_->OMSetDepthStencilState(DXStates::ds_depth_enable_no_write(), 0xff);
		break;
	case DEPTH_COMP_WRITE:
		device_context_->OMSetDepthStencilState(DXStates::ds_defalut(), 0xff);
		break;
	}
}

void RenderSystem::SetParticleCB(Particle& particle)
{
	cb_particle_.data.color		= particle.color;
	cb_particle_.data.values.x	= particle.timer;
	cb_particle_.data.values.y	= particle.frame_ratio;

	XMMATRIX s = XMMatrixScalingFromVector(XMLoadFloat3(&particle.scale));

	XMVECTOR rot_vec = { XMConvertToRadians(particle.rotation.x), XMConvertToRadians(particle.rotation.y), XMConvertToRadians(particle.rotation.z) };
	auto q = XMQuaternionRotationRollPitchYawFromVector(rot_vec);
	XMMATRIX r = XMMatrixRotationQuaternion(q);
	XMMATRIX t = XMMatrixTranslationFromVector(XMLoadFloat3(&particle.position));

	// SRT for no Billboard
	XMMATRIX sr = XMMatrixMultiply(s, r);
	XMMATRIX srt = XMMatrixMultiply(sr, t);

	cb_particle_.data.transform = XMMatrixTranspose(srt);

	// SRT for Billboard
	// multiply world rotation's inverse matrix
	XMVECTOR world_scale, world_rot, world_trans;
	XMMatrixDecompose(&world_scale, &world_rot, &world_trans, cb_effect_.data.world);
	auto world_inv_rot = XMMatrixRotationQuaternion(world_rot);
	r = XMMatrixMultiply(r, world_inv_rot);
	sr = XMMatrixMultiply(s, r);
	srt = XMMatrixMultiply(sr, t);

	cb_particle_.data.transform_for_billboard = XMMatrixTranspose(srt);

	device_context_->UpdateSubresource(cb_particle_.buffer.Get(), 0, nullptr, &cb_particle_.data, 0, 0);
	device_context_->GSSetConstantBuffers(3, 1, cb_particle_.buffer.GetAddressOf());
}

