#include "stdafx.h"
#include "InstancedObject.h"
#include "ResourceMgr.h"
#include "PhysicsMgr.h"

using namespace reality;

InstancedObject::InstancedObject()
{
}

InstancedObject::~InstancedObject()
{
}

void InstancedObject::Init(string stmesh_id, string vs_id)
{
	mesh_id_ = stmesh_id;
	vs_id_ = vs_id;

	static_mesh = shared_ptr<StaticMesh>(RESOURCE->UseResource<StaticMesh>(stmesh_id));
	vertex_shader = shared_ptr<VertexShader>(RESOURCE->UseResource<VertexShader>(vs_id));

	//DX11APP->GetDeviceContext()->UpdateSubresource(instancing.buffer.Get(), 0, 0, &instancing.data, 0, 0);

	object_name = split(stmesh_id, '.')[0];
}

void InstancedObject::Frame()
{
	if (instance_pool.empty())
		return;

	UpdateInstance();

	DX11APP->GetDeviceContext()->VSSetShader(nullptr, 0, 0);
	DX11APP->GetDeviceContext()->GSSetShader(nullptr, 0, 0);
	DX11APP->GetDeviceContext()->PSSetShader(nullptr, 0, 0);

	//DX11APP->GetDeviceContext()->UpdateSubresource(instancing.buffer.Get(), 0, 0, &instancing.data, 0, 0);
	//DX11APP->GetDeviceContext()->VSSetConstantBuffers(1, 1, instancing.buffer.GetAddressOf());
	DX11APP->GetDeviceContext()->IASetInputLayout(vertex_shader.get()->InputLayout());
	DX11APP->GetDeviceContext()->VSSetShader(vertex_shader.get()->Get(), 0, 0);

	if (inst_srv.Get())
		DX11APP->GetDeviceContext()->VSSetShaderResources(0, 1, inst_srv.GetAddressOf());
}

void InstancedObject::Render()
{
	if (instance_pool.empty())
		return;

	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;

	for (auto mesh : static_mesh->meshes)
	{
		reality::Material* material = RESOURCE->UseResource<reality::Material>(mesh.mesh_name + ".mat");
		if (material)
			material->Set();

		DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		DX11APP->GetDeviceContext()->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		DX11APP->GetDeviceContext()->DrawIndexedInstanced(mesh.indices.size(), instance_pool.size(), 0, 0, 0);
	}
}

void InstancedObject::Release()
{
	for (auto inst : instance_pool)
	{
		delete inst.second;
		inst.second = nullptr;
	}
}

bool reality::InstancedObject::CreateInstanceBuffer()
{
	auto data_array = GetCDataArray();
	if (data_array.size() <= 0)
		return false;

	inst_buffer.ReleaseAndGetAddressOf();
	inst_srv.ReleaseAndGetAddressOf();

	HRESULT hr;

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = sizeof(InstanceData::CData) * data_array.size();
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = sizeof(InstanceData::CData);

	D3D11_SUBRESOURCE_DATA subdata;
	subdata.pSysMem = data_array.data();
	subdata.SysMemPitch = 0;
	subdata.SysMemSlicePitch = 0;

	hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, inst_buffer.GetAddressOf());
	if (FAILED(hr))
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
	srv_desc.Format = DXGI_FORMAT_UNKNOWN;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srv_desc.Buffer.FirstElement = 0;
	srv_desc.Buffer.NumElements = data_array.size();

	hr = DX11APP->GetDevice()->CreateShaderResourceView(inst_buffer.Get(), &srv_desc, inst_srv.GetAddressOf());
	if (FAILED(hr))
		return false;

	return true;
}

vector<InstanceData::CData> reality::InstancedObject::GetCDataArray()
{
	vector<InstanceData::CData> data_array;
	for (auto data : instance_pool)
	{
		data_array.push_back(data.second->cdata);
	}

	return data_array;
}

void InstancedObject::AddNewInstance()
{
	InstanceData* data = new InstanceData(object_name, instance_pool.size());

	instance_pool.insert(make_pair(data->instance_id, data));

	auto current = instance_pool.find(data->instance_id)->second;
	selected_instance = current;

	CreateInstanceBuffer();
}

bool reality::InstancedObject::UpdateInstance()
{
	for (auto instance : instance_pool)
	{
		XMMATRIX transform = XMMatrixIdentity();
		transform *= TransformS(instance.second->S);
		transform *= TransformR(instance.second->R);
		transform *= TransformT(instance.second->T);

		instance.second->cdata.world_matrix = XMMatrixTranspose(transform);
	}

	auto data_array = GetCDataArray();

	HRESULT hr;

	D3D11_MAPPED_SUBRESOURCE mapped_resource;
	hr = DX11APP->GetDeviceContext()->Map(inst_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
	if (FAILED(hr))
		return false;
	
	size_t buffer_size = data_array.size() * sizeof(InstanceData::CData);
	memcpy_s(mapped_resource.pData, buffer_size, data_array.data(), buffer_size);

	DX11APP->GetDeviceContext()->Unmap(inst_buffer.Get(), 0);

	return true;
}