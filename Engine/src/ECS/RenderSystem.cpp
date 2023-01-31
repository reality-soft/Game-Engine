#include "RenderSystem.h"

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

void RenderSystem::OnCreate(entt::registry& reg)
{
	HRESULT hr;
	auto view_trs = reg.view<Transform>();
	auto view_skt = reg.view<Skeleton>();
	auto view_stm = reg.view<StaticMesh>();
	auto view_skm = reg.view<SkeletalMesh>();

	for (auto ent : view_trs)
	{
		auto& transform = reg.get<Transform>(ent);
		transform.world_matrix = transform.cb_transform.world_matrix = XMMatrixIdentity();
		transform.view_matrix = transform.cb_transform.world_matrix = XMMatrixIdentity();
		transform.projection_matrix = transform.cb_transform.world_matrix = XMMatrixIdentity();

		D3D11_BUFFER_DESC desc;
		D3D11_SUBRESOURCE_DATA subdata;

		ZeroMemory(&desc, sizeof(desc));
		ZeroMemory(&subdata, sizeof(subdata));

		desc.ByteWidth = sizeof(CbTransform);

		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		subdata.pSysMem = &transform.cb_transform;
		hr = device->CreateBuffer(&desc, &subdata, &transform.cb_buffer);
	}

	for (auto ent : view_skt)
	{
		auto& skeleton = view_skt.get<Skeleton>(ent);
		for (int i = 0; i < 255; ++i)
			skeleton.cb_skeleton.mat_skeleton[i] = XMMatrixIdentity();

		D3D11_BUFFER_DESC desc;
		D3D11_SUBRESOURCE_DATA subdata;

		ZeroMemory(&desc, sizeof(desc));
		ZeroMemory(&subdata, sizeof(subdata));

		desc.ByteWidth = sizeof(CbSkeleton);
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		subdata.pSysMem = skeleton.cb_buffer.GetAddressOf();

		hr = device->CreateBuffer(&desc, &subdata, skeleton.cb_buffer.GetAddressOf());
	}

	for (auto ent : view_skm)
	{
		auto& skm = view_skm.get<SkeletalMesh>(ent);
		for (auto& single_mesh : skm.mesh_list)
		{
			D3D11_BUFFER_DESC desc;
			D3D11_SUBRESOURCE_DATA subdata;

			ZeroMemory(&desc, sizeof(desc));
			ZeroMemory(&subdata, sizeof(subdata));

			desc.ByteWidth = sizeof(SkinnedVertex) * single_mesh.vertices.size();
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			subdata.pSysMem = single_mesh.vertices.data();

			hr = device->CreateBuffer(&desc, &subdata, single_mesh.vertex_buffer.GetAddressOf());
		}		
	}

	for (auto ent : view_stm)
	{
		auto& stm = view_stm.get<StaticMesh>(ent);
		for (auto& single_mesh : stm.mesh_list)
		{
			D3D11_BUFFER_DESC desc;
			D3D11_SUBRESOURCE_DATA subdata;

			ZeroMemory(&desc, sizeof(desc));
			ZeroMemory(&subdata, sizeof(subdata));

			desc.ByteWidth = sizeof(Vertex) * single_mesh.vertices.size();
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			subdata.pSysMem = single_mesh.vertices.data();

			hr = device->CreateBuffer(&desc, &subdata, single_mesh.vertex_buffer.GetAddressOf());
		}
	}
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
	device_context->PSSetShader(material.ps_default.Get(), 0, 0);

	if (material.srv_list.size() > 0)
		device_context->PSSetShaderResources(0, material.srv_list.size(), material.srv_list.data());
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
	for (auto single_mesh : static_mesh.mesh_list)
	{
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		device_context->IASetInputLayout(static_mesh.vs_default.InputLayoyt());
		device_context->VSSetShader(static_mesh.vs_default.Get(), 0, 0);
		device_context->IASetVertexBuffers(0, 1, single_mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		device_context->Draw(single_mesh.vertices.size(), 0);
	}
}

void RenderSystem::RenderSkeletalMesh(SkeletalMesh& skeletal_mesh)
{
	for (auto single_mesh : skeletal_mesh.mesh_list)
	{
		UINT stride = sizeof(SkinnedVertex);
		UINT offset = 0;
		device_context->IASetVertexBuffers(0, 1, single_mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		device_context->IASetInputLayout(skeletal_mesh.vs_skinned.InputLayoyt());
		device_context->VSSetShader(skeletal_mesh.vs_skinned.Get(), 0, 0);
		device_context->Draw(single_mesh.vertices.size(), 0);
	}
}
