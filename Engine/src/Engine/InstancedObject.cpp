#include "stdafx.h"
#include "InstancedObject.h"
#include "ResourceMgr.h"
#include "PhysicsMgr.h"

using namespace KGCA41B;

InstancedObject::InstancedObject()
{
}

InstancedObject::~InstancedObject()
{
}

void InstancedObject::Init(string stmesh_id, string vs_id)
{
	static_mesh = shared_ptr<StaticMesh>(RESOURCE->UseResource<StaticMesh>(stmesh_id));
	vs = shared_ptr<VertexShader>(RESOURCE->UseResource<VertexShader>(vs_id));

	CreateInstanceBuffer();
	DX11APP->GetDeviceContext()->UpdateSubresource(instancing.buffer.Get(), 0, 0, &instancing.data, 0, 0);

	object_name = split(stmesh_id, '.')[0];
}

void InstancedObject::Frame()
{
	if (instance_list.empty())
		return;

	UpdateInstance();

	DX11APP->GetDeviceContext()->VSSetShader(nullptr, 0, 0);
	DX11APP->GetDeviceContext()->GSSetShader(nullptr, 0, 0);
	DX11APP->GetDeviceContext()->PSSetShader(nullptr, 0, 0);

	DX11APP->GetDeviceContext()->UpdateSubresource(instancing.buffer.Get(), 0, 0, &instancing.data, 0, 0);
	DX11APP->GetDeviceContext()->VSSetConstantBuffers(1, 1, instancing.buffer.GetAddressOf());
	DX11APP->GetDeviceContext()->IASetInputLayout(vs.get()->InputLayout());
	DX11APP->GetDeviceContext()->VSSetShader(vs.get()->Get(), 0, 0);
}

void InstancedObject::Render()
{
	if (instance_list.empty())
		return;

	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;

	for (auto mesh : static_mesh->meshes)
	{
		KGCA41B::Material* material = RESOURCE->UseResource<KGCA41B::Material>(mesh.mesh_name + ".mat");
		if (material)
			material->Set();

		DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		DX11APP->GetDeviceContext()->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		DX11APP->GetDeviceContext()->DrawIndexedInstanced(mesh.indices.size(), instance_list.size(), 0, 0, 0);
	}
}

void InstancedObject::Release()
{
}

void InstancedObject::CreateInstanceBuffer()
{
	HRESULT hr;

	// Create a constant buffer for the instance data
	D3D11_SUBRESOURCE_DATA subdata;
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(CbInstance::Data);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	subdata.pSysMem = &instancing.data;

	hr = DX11APP->GetDevice()->CreateBuffer(&desc, NULL, instancing.buffer.GetAddressOf());
}

void InstancedObject::AddNewInstance()
{
	if (instance_list.size() > 128)
		return;

	reactphysics3d::Vector3 box_extend; XMtoRP(XMLoadFloat3(&static_mesh.get()->GetMaxXYZ()), box_extend);
	InstanceData data;

	//data.col_transform.identity();
	//data.box_shape = PHYSICS->physics_common_.createBoxShape(box_extend);
	//data.collision_body = PHYSICS->GetPhysicsWorld()->createCollisionBody(data.col_transform);
	//data.box_collider = data.collision_body->addCollider(data.box_shape, data.col_transform);

	data.index = instance_list.size();
	data.obj_name = object_name;

	instance_list.push_back(data);
	selected_instance = &instance_list.back();
}

InstanceData* KGCA41B::InstancedObject::FindAndSelectWithCollision(reactphysics3d::CollisionBody* col_body)
{
	for (auto& instance : instance_list)
	{
		if (instance.collision_body == col_body)
		{
			selected_instance = &instance;
		}
		else
		{
			selected_instance = nullptr;
		}
	}
	return selected_instance;
}

XMMATRIX InstancedObject::TransformS(XMFLOAT3& sacling)
{
	return XMMatrixScalingFromVector(XMLoadFloat3(&sacling));
}

XMMATRIX KGCA41B::InstancedObject::TransformR(XMFLOAT3& roation)
{
	XMMATRIX rotation = XMMatrixIdentity();
	rotation *= XMMatrixRotationX(roation.x);
	rotation *= XMMatrixRotationY(roation.y);
	rotation *= XMMatrixRotationZ(roation.z);

	return rotation;
}

XMMATRIX KGCA41B::InstancedObject::TransformT(XMFLOAT3& position)
{
	return XMMatrixTranslationFromVector(XMLoadFloat3(&position));
}

void KGCA41B::InstancedObject::UpdateInstance()
{
	for (auto instance : instance_list)
	{
		XMMATRIX transform = XMMatrixIdentity();
		transform *= TransformS(instance.S);
		transform *= TransformR(instance.R);
		transform *= TransformT(instance.T);

		instancing.data.instance_transform[instance.index] = XMMatrixTranspose(transform);
	}
	DX11APP->GetDeviceContext()->UpdateSubresource(instancing.buffer.Get(), 0, 0, &instancing.data, 0, 0);
}