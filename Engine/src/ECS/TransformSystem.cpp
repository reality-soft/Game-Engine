#include "stdafx.h"
#include "TransformSystem.h"

using namespace KGCA41B;


TransformSystem::TransformSystem()
{
}

TransformSystem::~TransformSystem()
{
}

void TransformSystem::OnCreate(entt::registry& reg)
{
	//this->cb_transform.world_matrix = XMMatrixIdentity();
	//this->cb_transform.view_matrix = XMMatrixIdentity();
	//this->cb_transform.projection_matrix = XMMatrixIdentity();

	//D3D11_BUFFER_DESC desc;
	//D3D11_SUBRESOURCE_DATA subdata;

	//ZeroMemory(&desc, sizeof(desc));
	//ZeroMemory(&subdata, sizeof(subdata));

	//desc.ByteWidth = sizeof(CbTransform);

	//desc.Usage = D3D11_USAGE_DEFAULT;
	//desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	//subdata.pSysMem = &this->cb_transform;
	//HRESULT hr;
	//hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, &this->cb_buffer);
}

void TransformSystem::OnUpdate(entt::registry& reg)
{

}

void TransformSystem::SetCbTransform(Transform& transform)
{
	//transform.cb_transform.world_matrix = XMMatrixTranspose(transform.local * transform.parent);
	//transform.cb_transform.view_matrix = XMMatrixTranspose(transform.view_matrix);
	//transform.cb_transform.projection_matrix = XMMatrixTranspose(transform.projection_matrix);

	//device_context->UpdateSubresource(transform.cb_buffer.Get(), 0, nullptr, &transform.cb_transform, 0, 0);
	//device_context->VSSetConstantBuffers(0, 1, transform.cb_buffer.GetAddressOf());
}