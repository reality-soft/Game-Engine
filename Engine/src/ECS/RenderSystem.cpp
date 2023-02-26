#include "stdafx.h"
#include "RenderSystem.h"
#include "ResourceMgr.h"
#include "TimeMgr.h"
#include "DataMgr.h"
#include "DataTypes.h"
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

	// Init Sprite Buffer
	ZeroMemory(&cb_sprite.data, sizeof(CbSprite::Data));

	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	cb_sprite.data.billboard_matrix = XMMatrixIdentity();

	desc.ByteWidth = sizeof(CbSprite::Data);

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	subdata.pSysMem = &cb_sprite.data;

	hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, cb_sprite.buffer.GetAddressOf());

	// Init Particle Buffer
	ZeroMemory(&cb_particle.data, sizeof(CbParticle::Data));

	cb_particle.data.values.x = 0.0f;
	cb_particle.data.values.y = 0.0f;
	cb_particle.data.values.z = 0.0f;
	cb_particle.data.values.w = 0.0f;

	cb_particle.data.color.x = 1.0f;
	cb_particle.data.color.y = 1.0f;
	cb_particle.data.color.z = 1.0f;
	cb_particle.data.color.w = 1.0f;

	cb_particle.data.transform = XMMatrixIdentity();

	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.ByteWidth = sizeof(CbParticle::Data);

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	subdata.pSysMem = &cb_particle.data;

	hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, cb_particle.buffer.GetAddressOf());
}

void RenderSystem::OnUpdate(entt::registry& reg)
{

	auto view_stm = reg.view<C_StaticMesh, C_Transform>();
	auto view_skm = reg.view<C_SkeletalMesh, C_Transform>();

	for (auto ent : view_stm)
	{
		auto& transform = reg.get<C_Transform>(ent);
		SetCbTransform(transform);

		auto& static_mesh = reg.get<C_StaticMesh>(ent);
		SetCbTransform(transform);
		RenderStaticMesh(static_mesh);
	}

	for (auto ent : view_skm)
	{
		auto& transform = reg.get<C_Transform>(ent);
		SetCbTransform(transform);

		auto& skeletal_mesh = reg.get<C_SkeletalMesh>(ent);
		auto& animation = reg.get<C_Animation>(ent);
		RenderSkeletalMesh(skeletal_mesh, animation);
	}

	// BoxShape Render
	auto view_box = reg.view<C_BoxShape, C_Transform>();
	for (auto ent : view_box)
	{
		auto& box = reg.get<C_BoxShape>(ent);
		SetCbTransform(box);

		//auto& material = reg.get<Material>(ent);
		//SetMaterial(material);

		RenderBoxShape(box);
	}

	// Emitter Render
	auto view_effect = reg.view<C_Effect>();
	for (auto ent : view_effect)
	{
		auto& effect = reg.get<C_Effect>(ent);

		for (auto& emitter : effect.emitters)
		{
			Sprite* sprite = RESOURCE->UseResource<Sprite>(emitter.sprite_id);
			if (sprite == nullptr)
				return;

			SetCbTransform(effect);

			// ��� ��� ���
			{
				auto view_camera = reg.view<C_Camera>();
				for (auto entity : view_camera)
				{
					auto& camera = view_camera.get<C_Camera>(entity);
					if (camera.tag == "Player")
					{
						XMVECTOR s, o, q, t;
						XMFLOAT3 position(camera.position.m128_f32[0], camera.position.m128_f32[1], camera.position.m128_f32[2]);

						s = XMVectorReplicate(1.0f);
						o = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
						q = XMQuaternionRotationRollPitchYaw(camera.pitch, camera.yaw, camera.roll);
						t = XMLoadFloat3(&position);
						auto world_matrix = XMMatrixAffineTransformation(s, o, q, t);
						auto view_matrix = XMMatrixInverse(0, world_matrix);
						XMVECTOR determinant;
						XMMATRIX mat_view_inverse = XMMatrixInverse(&determinant, view_matrix);
						mat_view_inverse.r[3].m128_f32[0] = 0.0f;
						mat_view_inverse.r[3].m128_f32[1] = 0.0f;
						mat_view_inverse.r[3].m128_f32[2] = 0.0f;
						cb_sprite.data.billboard_matrix = XMMatrixTranspose(mat_view_inverse);
					}
				}
			}
			

			SetSprite(sprite);

			// ���̴� ����
			VertexShader* vs = RESOURCE->UseResource<VertexShader>(emitter.vs_id);
			if (vs)
			{
				device_context->IASetInputLayout(vs->InputLayoyt());
				device_context->VSSetShader(vs->Get(), 0, 0);
			}

			//GeometryShader* gs = RESOURCE->UseResource<GeometryShader>(emitter.geo_id);
			//if(gs)
			//	device_context->GSSetShader(gs->Get(), 0, 0);

			PixelShader* ps = RESOURCE->UseResource<PixelShader>(emitter.ps_id);
			if (ps)
				device_context->PSSetShader(ps->Get(), 0, 0);

			// particle ���� ��ŭ ��ƼŬ ���
			for (auto& particle : emitter.particles)
			{
				if (!particle.enable)
					continue;

				if (sprite->type == TEX)
				{
					TextureSprite* tex_sprite = (TextureSprite*)sprite;
					Texture* texture = RESOURCE->UseResource<Texture>(tex_sprite->tex_id_list[particle.timer]);
					if (texture != nullptr)
						device_context->PSSetShaderResources(0, 1, texture->srv.GetAddressOf());
				}

				SetParticle(particle);

				RenderParticle(particle);
			}
		}

		
	}
}

void RenderSystem::SetCbTransform(const C_Transform& transform)
{
	cb_transform.data.world_matrix = XMMatrixTranspose(transform.world * transform.local);

	device_context->UpdateSubresource(cb_transform.buffer.Get(), 0, nullptr, &cb_transform.data, 0, 0);
	device_context->VSSetConstantBuffers(0, 1, cb_transform.buffer.GetAddressOf());
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

void RenderSystem::RenderStaticMesh(C_StaticMesh& static_mesh_component)
{
	StaticMesh* static_mesh = RESOURCE->UseResource<StaticMesh>(static_mesh_component.static_mesh_id);
	VertexShader* shader = RESOURCE->UseResource<VertexShader>(static_mesh_component.vertex_shader_id);

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

		device_context->IASetInputLayout(shader->InputLayoyt());
		device_context->VSSetShader(shader->Get(), 0, 0);

		device_context->DrawIndexed(single_mesh.indices.size(), 0, 0);
	}

}

void RenderSystem::RenderSkeletalMesh(const C_SkeletalMesh& skeletal_mesh_components, const C_Animation& animation_component)
{
	SkeletalMesh* skeletal_mesh = RESOURCE->UseResource<SkeletalMesh>(skeletal_mesh_components.skeletal_mesh_id);
	VertexShader* shader = RESOURCE->UseResource<VertexShader>(skeletal_mesh_components.vertex_shader_id);
	vector<OutAnimData>* res_animation = RESOURCE->UseResource<vector<OutAnimData>>(animation_component.anim_id);
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
		
		device_context->IASetInputLayout(shader->InputLayoyt());
		device_context->VSSetShader(shader->Get(), 0, 0);

		device_context->DrawIndexed(single_mesh.indices.size(), 0, 0);
	}
}

void RenderSystem::RenderBoxShape(C_BoxShape& box_shape)
{
	VertexShader* shader = RESOURCE->UseResource<VertexShader>(box_shape.vs_id);
	if (shader == nullptr)
		return;


	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	device_context->IASetVertexBuffers(0, 1, box_shape.vertex_buffer.GetAddressOf(), &stride, &offset);
	device_context->IASetIndexBuffer(box_shape.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	device_context->IASetInputLayout(shader->InputLayoyt());
	device_context->VSSetShader(shader->Get(), 0, 0);


	device_context->DrawIndexed(box_shape.index_list.size(), 0, 0);
}

void RenderSystem::SetParticle(Particle& particle)
{
	cb_particle.data.color		= particle.color;
	cb_particle.data.values.x	= particle.timer;
	cb_particle.data.values.y	= particle.frame_ratio;

	XMMATRIX s = XMMatrixScalingFromVector(XMLoadFloat3(&particle.scale));
	auto q = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&particle.rotation));;
	XMMATRIX r = XMMatrixRotationQuaternion(q);
	XMMATRIX t = XMMatrixTranslationFromVector(XMLoadFloat3(&particle.position));
	XMMATRIX sr = XMMatrixMultiply(s, r);
	XMMATRIX srt = XMMatrixMultiply(sr, t);

	cb_particle.data.transform = XMMatrixTranspose(srt);

	device_context->UpdateSubresource(cb_particle.buffer.Get(), 0, nullptr, &cb_particle.data, 0, 0);
	device_context->VSSetConstantBuffers(3, 1, cb_particle.buffer.GetAddressOf());
}

void RenderSystem::SetSprite(Sprite* sprite)
{
	// max_frame ���
	cb_sprite.data.value.y = sprite->max_frame;
	switch (sprite->type)
	{
	// UV
	case UV:
	{
		// type
		cb_sprite.data.value.x = 0;


		UVSprite* uv_sprite = (UVSprite*)sprite;

		// SRV ���
		Texture* texture = RESOURCE->UseResource<Texture>(uv_sprite->tex_id);
		if (texture != nullptr)
			device_context->PSSetShaderResources(0, 1, texture->srv.GetAddressOf());

		cb_sprite.data.value.z = uv_sprite->uv_list.size();

		for (int i = 0; i < uv_sprite->uv_list.size(); i++)
		{
			cb_sprite.data.value2[i].x = (float)uv_sprite->uv_list[i].first.x / (float)texture->texture_desc.Width;
			cb_sprite.data.value2[i].y = (float)uv_sprite->uv_list[i].first.y / (float)texture->texture_desc.Height;
			cb_sprite.data.value2[i].z = (float)uv_sprite->uv_list[i].second.x / (float)texture->texture_desc.Width;
			cb_sprite.data.value2[i].w = (float)uv_sprite->uv_list[i].second.y / (float)texture->texture_desc.Height;
		}
	}break;
	// Tex
	case TEX:
	{
		// type
		cb_sprite.data.value.x = 1;


	}break;
	}

	device_context->UpdateSubresource(cb_sprite.buffer.Get(), 0, nullptr, &cb_sprite.data, 0, 0);
	device_context->VSSetConstantBuffers(2, 1, cb_sprite.buffer.GetAddressOf());
}

void RenderSystem::RenderParticle(Particle& particle)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	device_context->IASetVertexBuffers(0, 1, particle.vertex_buffer.GetAddressOf(), &stride, &offset);
	device_context->IASetIndexBuffer(particle.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	device_context->DrawIndexed(particle.index_list.size(), 0, 0);
}


