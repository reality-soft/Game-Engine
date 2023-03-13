#include "stdafx.h"
#include "UIBase.h"
#include "Engine.h"
#include "DX11App.h"
#include "ResourceMgr.h"

using namespace reality;

void UIBase::Init()
{
	CreateRenderData();
}

void UIBase::Update()
{
	UpdateThisUI();
	for (auto child : child_ui_list_)
	{
		child->Update();
	}
}

void UIBase::Render()
{
	RenderThisUI();
	for (auto child : child_ui_list_)
	{
		child->Render();
	}
}

void UIBase::CreateRenderData()
{
	// 쉐이더 이름 입력
	render_data_.vs_id = "UIVS.cso";
	render_data_.ps_id = "UIPS.cso";

	// 정점 작성
	render_data_.vertex_list.push_back({ { -1.0f, +1.0f }, {+0.0f, +0.0f} });
	render_data_.vertex_list.push_back({ { +1.0f, +1.0f }, {+1.0f, +0.0f} });
	render_data_.vertex_list.push_back({ { -1.0f, -1.0f }, {+0.0f, +1.0f} });
	render_data_.vertex_list.push_back({ { +1.0f, -1.0f }, {+1.0f, +1.0f} });

	D3D11_BUFFER_DESC bufDesc;

	ZeroMemory(&bufDesc, sizeof(D3D11_BUFFER_DESC));

	bufDesc.ByteWidth = sizeof(UIVertex) * render_data_.vertex_list.size();
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = 0;
	bufDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subResourse;

	ZeroMemory(&subResourse, sizeof(D3D11_SUBRESOURCE_DATA));

	subResourse.pSysMem = &render_data_.vertex_list.at(0);
	subResourse.SysMemPitch;
	subResourse.SysMemSlicePitch;

	DX11APP->GetDevice()->CreateBuffer(&bufDesc, &subResourse, render_data_.vertex_buffer.GetAddressOf());

	// 인덱스 작성

	render_data_.index_list.push_back(0);
	render_data_.index_list.push_back(1);
	render_data_.index_list.push_back(2);
	render_data_.index_list.push_back(2);
	render_data_.index_list.push_back(1);
	render_data_.index_list.push_back(3);

	ZeroMemory(&bufDesc, sizeof(D3D11_BUFFER_DESC));

	bufDesc.ByteWidth = sizeof(DWORD) * render_data_.index_list.size();
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = 0;
	bufDesc.StructureByteStride = 0;

	ZeroMemory(&subResourse, sizeof(D3D11_SUBRESOURCE_DATA));

	subResourse.pSysMem = &render_data_.index_list.at(0);
	subResourse.SysMemPitch;
	subResourse.SysMemSlicePitch;

	DX11APP->GetDevice()->CreateBuffer(&bufDesc, &subResourse, render_data_.index_buffer.GetAddressOf());
}

void UIBase::UpdateThisUI()
{
	float scale_x = ui_rect_.width / ENGINE->GetWindowSize().x;
	float scale_y = ui_rect_.height / ENGINE->GetWindowSize().y;
	float pos_x = ui_rect_.center.x / ENGINE->GetWindowSize().x;
	float pos_y = ui_rect_.center.y / ENGINE->GetWindowSize().y;

	XMMATRIX s = XMMatrixScaling(scale_x, scale_y, 1.0f);
	XMMATRIX r = XMMatrixRotationZ(0.0f);
	XMMATRIX t = XMMatrixTranslation(pos_x, pos_y, 0.0f);

	ui_transform_.local =  s * r * t;

	if (parent_ui_ != NULL)
	{
		ui_transform_.world = parent_ui_->ui_transform_.world * ui_transform_.local;
	}
	else
	{
		ui_transform_.world = ui_transform_.local;
	}
}

void UIBase::RenderThisUI()
{
	// Set Topology
	DX11APP->GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set Vertex Buffer
	UINT stride = sizeof(UIVertex);
	UINT offset = 0;
	DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, render_data_.vertex_buffer.GetAddressOf(), &stride, &offset);

	// Set Index Buffer
	DX11APP->GetDeviceContext()->IASetIndexBuffer(render_data_.index_buffer.Get(), DXGI_FORMAT_R32_UINT, offset);

	// Get Shader From ResourceMgr
	VertexShader* vs = RESOURCE->UseResource<VertexShader>(render_data_.vs_id);
	PixelShader* ps = RESOURCE->UseResource<PixelShader>(render_data_.ps_id);

	if (vs == nullptr || ps == nullptr)
		return;

	// Set InputLayout
	DX11APP->GetDeviceContext()->IASetInputLayout(vs->InputLayout());

	// Set VS
	DX11APP->GetDeviceContext()->VSSetShader(vs->Get(), 0, 0);

	// Set PS
	DX11APP->GetDeviceContext()->PSSetShader(ps->Get(), 0, 0);

	// Reset GS
	DX11APP->GetDeviceContext()->GSSetShader(nullptr, 0, 0);

	// Get Texture From ResourceMgr
	Texture* texture = RESOURCE->UseResource<Texture>(render_data_.tex_id);

	if (texture == nullptr)
		return;

	// Set Texture
	DX11APP->GetDeviceContext()->PSSetShaderResources(0, 1, texture->srv.GetAddressOf());

	// Draw Indexed
	DX11APP->GetDeviceContext()->DrawIndexed(render_data_.index_list.size(), 0, 0);
}

void UIBase::SetRectByMin(XMFLOAT2 min, float width, float height)
{
	ui_rect_.min = min;
	ui_rect_.width = width;
	ui_rect_.height = height;
	ui_rect_.max = { min.x + width, min.y + height };
	ui_rect_.center = { min.x + width / 2.0f, min.y + height / 2.0f };
}

void UIBase::SetRectByMax(XMFLOAT2 max, float width, float height)
{
	ui_rect_.max = max;
	ui_rect_.width = width;
	ui_rect_.height = height;
	ui_rect_.min = { max.x - width, max.y - height };
	ui_rect_.center = { max.x - width / 2.0f, max.y - height / 2.0f };
}

void UIBase::SetRectByCenter(XMFLOAT2 center, float width, float height)
{
	ui_rect_.center = center;
	ui_rect_.width = width;
	ui_rect_.height = height;
	ui_rect_.min = { center.x - width / 2.0f, center.y - width / 2.0f };
	ui_rect_.max = { center.x + width / 2.0f, center.y + width / 2.0f };
}

void UIBase::SetRectByMinMax(XMFLOAT2 min, XMFLOAT2 max)
{
	ui_rect_.min = min;
	ui_rect_.max = max;
	ui_rect_.width = max.x - min.x;
	ui_rect_.height = max.y - min.y;
	ui_rect_.center = { min.x + (ui_rect_.width / 2.0f), min.y + (ui_rect_.height / 2.0f) };
}