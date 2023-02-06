#include "RenderSystem.h"
#include "ResourceMgr.h"
#include "TimeMgr.h"
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
}

void RenderSystem::SetMaterial(Material& material)
{
	PsDefault* shader = RESOURCE->UseResource<PsDefault>(material.shader_id);
	Texture* texture = RESOURCE->UseResource<Texture>(material.texture_id);

	device_context->PSSetShader(shader->Get(), 0, 0);

	if (texture != nullptr && texture->srv_list.size() > 0)
		device_context->PSSetShaderResources(0, texture->srv_list.size(), texture->srv_list.data());
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
	VsDefault* shader = RESOURCE->UseResource<VsDefault>(static_mesh.shader_id);

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
	VsSkinned* shader = RESOURCE->UseResource<VsSkinned>(skeletal_mesh.shader_id);

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
