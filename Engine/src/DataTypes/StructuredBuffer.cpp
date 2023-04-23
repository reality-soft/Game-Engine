#include "stdafx.h"
#include "DX11App.h"
#include "StructuredBuffer.h"


using namespace reality;

bool reality::StructuredSRV::Create(void* p_data)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC buffer_desc;
	D3D11_SUBRESOURCE_DATA subdata;

	ZeroMemory(&buffer_desc, sizeof(buffer_desc));
	ZeroMemory(&subdata, sizeof(subdata));
	buffer_desc.ByteWidth = byte_width;
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	buffer_desc.StructureByteStride = byte_stride;

	subdata.pSysMem = p_data;
	subdata.SysMemPitch = 0;
	subdata.SysMemSlicePitch = 0;

	hr = DX11APP->GetDevice()->CreateBuffer(&buffer_desc, &subdata, buffer.GetAddressOf());
	if (FAILED(hr))
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
	ZeroMemory(&srv_desc, sizeof(srv_desc));

	srv_desc.Format = DXGI_FORMAT_UNKNOWN;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srv_desc.Buffer.ElementWidth = byte_stride;
	srv_desc.Buffer.NumElements = byte_width / byte_stride;

	hr = DX11APP->GetDevice()->CreateShaderResourceView(buffer.Get(), &srv_desc, srv.GetAddressOf());
	if (FAILED(hr))
		return false;

	return true;
}

bool reality::StructuredUAV::Create(void* p_data)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC buffer_desc;
	D3D11_SUBRESOURCE_DATA subdata;

	ZeroMemory(&buffer_desc, sizeof(buffer_desc));
	ZeroMemory(&subdata, sizeof(subdata));
	buffer_desc.ByteWidth = byte_width;
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	buffer_desc.StructureByteStride = byte_stride;

	subdata.pSysMem = p_data;
	subdata.SysMemPitch = 0;
	subdata.SysMemSlicePitch = 0;

	hr = DX11APP->GetDevice()->CreateBuffer(&buffer_desc, &subdata, buffer.GetAddressOf());
	if (FAILED(hr))
		return false;

	D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
	ZeroMemory(&uav_desc, sizeof(uav_desc));

	uav_desc.Format = DXGI_FORMAT_UNKNOWN;
	uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uav_desc.Buffer.NumElements = byte_width / byte_stride;

	hr = DX11APP->GetDevice()->CreateUnorderedAccessView(buffer.Get(), &uav_desc, uav.GetAddressOf());
	if (FAILED(hr))
		return false;

	return true;
}

void reality::SbTriangleCollision::SetElementArraySize(UINT size)
{
	elements.resize(size);

	for (auto& e : elements)
	{
		e.including_node = 0;
		e.normal = { 0, 0, 0 };
		e.vertex0 = { 0, 0, 0 };
		e.vertex1 = { 0, 0, 0 };
		e.vertex2 = { 0, 0, 0 };
	}

	byte_stride = sizeof(SbTriangleCollision::Data);
	byte_width = elements.size() * byte_stride;
}

void reality::SbCapsuleCollision::SetElementArraySize(UINT size)
{
	elements.resize(size);

	for (auto& e : elements)
	{
		e.radius = 0;
		e.point_a = { 0 ,0, 0 };
		e.point_b = { 0, 0, 0 };
	}

	byte_stride = sizeof(SbCapsuleCollision::Data);
	byte_width = elements.size() * byte_stride;
}

void reality::SbSphereCollision::SetElementArraySize(UINT size)
{
	elements.resize(size);

	for (auto& e : elements)
	{
		e.radius = 0;
		e.center = { 0 ,0, 0 };
	}

	byte_stride = sizeof(SbSphereCollision::Data);
	byte_width = elements.size() * byte_stride;
}


void reality::SbCollisionResult::SetElementArraySize(UINT size)
{
	elements.resize(size);

	for (auto& e : elements)
	{
		e.capsule_result.entity = 0;
		e.capsule_result.collide_type = 0;
		e.capsule_result.floor_position = { 0, 0, 0 };
		ZeroMemory(&e.capsule_result.wall_planes, sizeof(e.capsule_result.wall_planes));

		e.sphere_result.entity = 0;
		e.sphere_result.tri_normal = { 0, 0, 0 };
	}

	byte_stride = sizeof(SbCollisionResult::Data);
	byte_width = elements.size() * byte_stride;
}