#include "stdafx.h"
#include "InstancedObject.h"
#include "ResourceMgr.h"

using namespace reality;

InstancedObject::InstancedObject()
{
}

InstancedObject::~InstancedObject()
{
}

bool InstancedObject::Init(string stmesh_id, string vs_id, string mat_id)
{
	static_mesh = shared_ptr<StaticMesh>(RESOURCE->UseResource<StaticMesh>(stmesh_id));
	vertex_shader = shared_ptr<VertexShader>(RESOURCE->UseResource<VertexShader>(vs_id));
	material = shared_ptr<Material>(RESOURCE->UseResource<Material>(mat_id));

	if (static_mesh.get() == nullptr)
		return false;

	if (vertex_shader.get() == nullptr)
		return false;

	if (material.get() == nullptr)
		return false;

	object_name = split(stmesh_id, '.')[0];

	return true;
}

void InstancedObject::Frame()
{
	if (instance_pool.empty())
		return;

	UpdateInstance();

	DX11APP->GetDeviceContext()->VSSetShader(nullptr, 0, 0);
	DX11APP->GetDeviceContext()->GSSetShader(nullptr, 0, 0);
	DX11APP->GetDeviceContext()->PSSetShader(nullptr, 0, 0);
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

	for (auto& mesh : static_mesh->meshes)
	{
		material->Set();

		DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		DX11APP->GetDeviceContext()->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		DX11APP->GetDeviceContext()->DrawIndexedInstanced(mesh.indices.size(), instance_pool.size(), 0, 0, 0);
	}
}

void InstancedObject::Release()
{
	for (auto& inst : instance_pool)
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

InstanceData* reality::InstancedObject::SelectInstance(UINT index)
{
	const auto& found = instance_pool.find(index);
	if (found == instance_pool.end())
		return nullptr;

	selected_instance = found->second;
	return selected_instance;
}

void reality::InstancedObject::SetInstanceScale(UINT index, XMFLOAT3 S)
{
	auto found = instance_pool.find(index);
	if (found != instance_pool.end())
	{
		found->second->S = S;
	}
}

void reality::InstancedObject::SetInstanceRotation(UINT index, XMFLOAT3 R)
{
	auto found = instance_pool.find(index);
	if (found != instance_pool.end())
	{
		found->second->R = R;
	}
}

void reality::InstancedObject::SetInstanceTranslation(UINT index, XMFLOAT3 T)
{
	auto found = instance_pool.find(index);
	if (found != instance_pool.end())
	{
		found->second->T = T;
	}
}

InstanceData* InstancedObject::AddNewInstance(string name)
{
	UINT index = instance_pool.size();
	InstanceData* data = new InstanceData(name, index);

	instance_pool.insert(make_pair(index, data));

	auto current = instance_pool.find(index)->second;
	selected_instance = current;

	CreateInstanceBuffer();

	return data;
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