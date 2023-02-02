#include "RenderSystem.h"
#include "ResourceMgr.h"
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

void RenderSystem::OnUpdate(entt::registry& reg)
{
	auto view_trs = reg.view<Transform>();
	auto view_skt = reg.view<Skeleton>();
	auto view_stm = reg.view<Material, StaticMesh>();
	auto view_skm = reg.view<Material, SkeletalMesh>();

	for (auto ent : view_trs)
	{
		auto& transform = reg.get<Transform>(ent);
		transform.world_matrix = XMMatrixRotationX(-90) * XMMatrixScaling(10, 10, 10);

		SetCbTransform(transform);
	}

	for (auto ent : view_skt)
	{
		auto& skeleton = reg.get<Skeleton>(ent);
		SetCbSkeleton(skeleton);
	}

	for (auto ent : view_stm)
	{
		auto& material = reg.get<Material>(ent);
		SetMaterial(material);

		auto& static_mesh = reg.get<StaticMesh>(ent);
		RenderStaticMesh(static_mesh);
	}

	for (auto ent : view_skm)
	{
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
	transform.cb_transform.world_matrix = XMMatrixTranspose(transform.world_matrix);
	transform.cb_transform.view_matrix = XMMatrixTranspose(transform.view_matrix);
	transform.cb_transform.projection_matrix = XMMatrixTranspose(transform.projection_matrix);

	device_context->UpdateSubresource(transform.cb_buffer.Get(), 0, nullptr, &transform.cb_transform, 0, 0);
	device_context->VSSetConstantBuffers(0, 1, transform.cb_buffer.GetAddressOf());
}

void RenderSystem::SetCbSkeleton(Skeleton& skeleton)
{
	device_context->UpdateSubresource(skeleton.cb_buffer.Get(), 0, nullptr, &skeleton.cb_skeleton, 0, 0);
	device_context->VSSetConstantBuffers(1, 1, skeleton.cb_buffer.GetAddressOf());
}

void RenderSystem::RenderStaticMesh(StaticMesh& static_mesh)
{
	vector<SingleMesh<Vertex>>* mesh_list = RESOURCE->UseResource<vector<SingleMesh<Vertex>>>(static_mesh.mesh_id);
	VsDefault* shader = RESOURCE->UseResource<VsDefault>(static_mesh.shader_id);

	for (auto single_mesh : *mesh_list)
	{
		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		device_context->IASetInputLayout(shader->InputLayoyt());
		device_context->VSSetShader(shader->Get(), 0, 0);
		device_context->IASetVertexBuffers(0, 1, single_mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		device_context->Draw(single_mesh.vertices.size(), 0);
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
		device_context->IASetInputLayout(shader->InputLayoyt());
		device_context->VSSetShader(shader->Get(), 0, 0);
		device_context->Draw(single_mesh.vertices.size(), 0);
	}
}
