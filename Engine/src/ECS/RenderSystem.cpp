#include "RenderSystem.h"

using namespace KGCA41B;

KGCA41B::RenderSystem::RenderSystem()
{
	device = DX11APP->GetDevice();
	device_context = DX11APP->GetDeviceContext();
}

KGCA41B::RenderSystem::~RenderSystem()
{
	device = nullptr;
	device_context = nullptr;
}

void KGCA41B::RenderSystem::OnCreate(entt::registry reg)
{

}

void KGCA41B::RenderSystem::OnUpdate(entt::registry reg)
{
	auto view_trs = reg.view<Transform>();
	auto view_skt = reg.view<Skeleton>();
	auto view_stm = reg.view<Material, StaticMesh>();
	auto view_skm = reg.view<Material, SkeletalMesh>();

	for (auto ent : view_trs)
	{
		auto& transform = reg.get<Transform>(ent);
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

void KGCA41B::RenderSystem::SetMaterial(Material& material)
{
	device_context->PSSetShader(material.ps_default.Get(), 0, 0);

	if (material.srv_list.size() > 0)
		device_context->PSSetShaderResources(0, material.srv_list.size(), material.srv_list.data());
}

void KGCA41B::RenderSystem::SetCbTransform(Transform& transform)
{
	transform.cb_transform.world_matrix = XMMatrixTranspose(transform.world_matrix);
	transform.cb_transform.view_matrix = XMMatrixTranspose(transform.view_matrix);
	transform.cb_transform.projection_matrix = XMMatrixTranspose(transform.projection_matrix);

	device_context->UpdateSubresource(transform.cb_buffer.Get(), 0, nullptr, &transform.cb_transform, 0, 0);
	device_context->VSSetConstantBuffers(0, 1, transform.cb_buffer.GetAddressOf());
}

void KGCA41B::RenderSystem::SetCbSkeleton(Skeleton& skeleton)
{
	device_context->UpdateSubresource(skeleton.cb_buffer.Get(), 0, nullptr, &skeleton.cb_skeleton, 0, 0);
	device_context->VSSetConstantBuffers(1, 1, skeleton.cb_buffer.GetAddressOf());
}

void KGCA41B::RenderSystem::RenderStaticMesh(StaticMesh& static_mesh)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	device_context->IASetInputLayout(static_mesh.vs_default.InputLayoyt());
	device_context->VSSetShader(static_mesh.vs_default.Get(), 0, 0);

	for (auto single_mesh : static_mesh.mesh_list)
	{
		device_context->IASetVertexBuffers(0, 1, &single_mesh.vertex_buffer, &stride, &offset);
		device_context->Draw(single_mesh.vertices.size(), 0);
	}
}

void KGCA41B::RenderSystem::RenderSkeletalMesh(SkeletalMesh& skeletal_mesh)
{
	UINT stride = sizeof(SkinnedVertex);
	UINT offset = 0;
	device_context->IASetInputLayout(skeletal_mesh.vs_skinned.InputLayoyt());
	device_context->VSSetShader(skeletal_mesh.vs_skinned.Get(), 0, 0);

	for (auto single_mesh : skeletal_mesh.mesh_list)
	{
		device_context->IASetVertexBuffers(0, 1, &single_mesh.vertex_buffer, &stride, &offset);
		device_context->Draw(single_mesh.vertices.size(), 0);
	}
}
