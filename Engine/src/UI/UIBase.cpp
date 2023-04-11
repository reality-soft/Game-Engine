#include "stdafx.h"
#include "UIBase.h"
#include "Engine.h"
#include "ResourceMgr.h"
#include "UISystem.h"

using namespace reality;

void UIBase::Init()
{
	current_state_ = E_UIState::UI_NORMAL;
	On();
	CreateRenderData();
	rect_transform_.resize(E_Resolution::RESOLUTION_COUNT);
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
	UpdateRectTransform();
}

void UIBase::RenderThisUI()
{
	// Set Constant Buffer
	auto resolution = ENGINE->GetWindowResolution();
	UISystem::SetCbData(XMMatrixTranspose(rect_transform_[resolution].world_matrix));

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

void UIBase::UpdateRectTransform()
{
	//auto resolution = ENGINE->GetWindowResolution();
	for (int i = 0; i < E_Resolution::RESOLUTION_COUNT; i++)
	{
		// 부모 UI가 있으면 부모 UI의 좌표계에서 계산
		if (parent_ui_ != NULL)
		{
			XMFLOAT2 world_min = {
				rect_transform_[i].local_rect.min.x + parent_ui_->rect_transform_[i].world_rect.min.x,
				rect_transform_[i].local_rect.min.y + parent_ui_->rect_transform_[i].world_rect.min.y,
			};

			rect_transform_[i].world_rect.SetRectByMin(world_min, rect_transform_[i].local_rect.width, rect_transform_[i].local_rect.height);

			float scale_x = rect_transform_[i].world_rect.width / E_Resolution_Size[i].x;  //ENGINE->GetWindowSize().x;
			float scale_y = rect_transform_[i].world_rect.height / E_Resolution_Size[i].y; //ENGINE->GetWindowSize().y;
			float pos_x = rect_transform_[i].world_rect.center.x / E_Resolution_Size[i].x; //ENGINE->GetWindowSize().x;
			float pos_y = rect_transform_[i].world_rect.center.y / E_Resolution_Size[i].y; //ENGINE->GetWindowSize().y;

			XMMATRIX s = XMMatrixScaling(scale_x, scale_y, 1.0f);
			XMMATRIX r = XMMatrixRotationZ(0.0f);
			XMMATRIX t = XMMatrixTranslation(pos_x, pos_y, 0.0f);

			rect_transform_[i].world_matrix = s * r * t;
		}
		else
		{
			float scale_x = rect_transform_[i].local_rect.width / E_Resolution_Size[i].x;  // ENGINE->GetWindowSize().x;
			float scale_y = rect_transform_[i].local_rect.height / E_Resolution_Size[i].y; // ENGINE->GetWindowSize().y;
			float pos_x = rect_transform_[i].local_rect.center.x / E_Resolution_Size[i].x; // ENGINE->GetWindowSize().x;
			float pos_y = rect_transform_[i].local_rect.center.y / E_Resolution_Size[i].y; // ENGINE->GetWindowSize().y;

			XMMATRIX s = XMMatrixScaling(scale_x, scale_y, 1.0f);
			XMMATRIX r = XMMatrixRotationZ(0.0f);
			XMMATRIX t = XMMatrixTranslation(pos_x, pos_y, 0.0f);

			rect_transform_[i].local_matrix = s * r * t;

			rect_transform_[i].world_rect = rect_transform_[i].local_rect;
			rect_transform_[i].world_matrix = rect_transform_[i].local_matrix;
		}
	}
	
}

void UIBase::AddChildUI(shared_ptr<UIBase> child_ui)
{
	child_ui_list_.insert(child_ui);
	child_ui->parent_ui_ = this;
}

void reality::UIBase::DeleteChildUI(shared_ptr<UIBase> child_ui)
{
	child_ui_list_.erase(child_ui);
	child_ui->parent_ui_ = nullptr;
}

E_UIState UIBase::GetCurrentState()
{
	return current_state_;
}

void UIBase::SetCurrentState(E_UIState state)
{
	current_state_ = state;
}

void UIBase::On()
{
	onoff_ = true;
}
void UIBase::Off()
{
	onoff_ = false;
}

bool UIBase::GetOnOff()
{
	return onoff_;
}

void UIBase::SetLocalRectByMin(XMFLOAT2 min, float width, float height)
{
	for (int i = 0; i < E_Resolution::RESOLUTION_COUNT; i++)
	{
		float resolution_multiply = E_Resolution_Multiply[i];
		XMFLOAT2 resolution_min = { min.x * resolution_multiply, min.y * resolution_multiply };
		ComputeLocalRectByMin((E_Resolution)i, resolution_min, width * resolution_multiply, height * resolution_multiply);
	}

	UpdateRectTransform();
}

void UIBase::SetLocalRectByMax(XMFLOAT2 max, float width, float height)
{
	for (int i = 0; i < E_Resolution::RESOLUTION_COUNT; i++)
	{
		float resolution_multiply = E_Resolution_Multiply[i];
		XMFLOAT2 resolution_max = { max.x * resolution_multiply, max.y * resolution_multiply };
		ComputeLocalRectByMax((E_Resolution)i, resolution_max, width * resolution_multiply, height * resolution_multiply);
	}

	UpdateRectTransform();
}

void UIBase::SetLocalRectByCenter(XMFLOAT2 center, float width, float height)
{
	for (int i = 0; i < E_Resolution::RESOLUTION_COUNT; i++)
	{
		float resolution_multiply = E_Resolution_Multiply[i];
		XMFLOAT2 resolution_center = { center.x * resolution_multiply, center.y * resolution_multiply };
		ComputeLocalRectByCenter((E_Resolution)i, resolution_center, width * resolution_multiply, height * resolution_multiply);
	}

	UpdateRectTransform();
}

void UIBase::SetLocalRectByMinMax(XMFLOAT2 min, XMFLOAT2 max)
{
	for (int i = 0; i < E_Resolution::RESOLUTION_COUNT; i++)
	{
		float resolution_multiply = E_Resolution_Multiply[i];
		XMFLOAT2 resolution_min = { min.x * resolution_multiply, min.y * resolution_multiply };
		XMFLOAT2 resolution_max = { max.x * resolution_multiply, max.y * resolution_multiply };
		ComputeLocalRectByMinMax((E_Resolution)i, resolution_min, resolution_max);
	}

	UpdateRectTransform();
}

void UIBase::ComputeLocalRectByMin(E_Resolution resolution, XMFLOAT2 min, float width, float height)
{
	rect_transform_[resolution].local_rect.min = min;
	rect_transform_[resolution].local_rect.width = width;
	rect_transform_[resolution].local_rect.height = height;
	rect_transform_[resolution].local_rect.max = { min.x + width, min.y + height };
	rect_transform_[resolution].local_rect.center = { min.x + width / 2.0f, min.y + height / 2.0f };
}

void UIBase::ComputeLocalRectByMax(E_Resolution resolution, XMFLOAT2 max, float width, float height)
{
	rect_transform_[resolution].local_rect.max = max;
	rect_transform_[resolution].local_rect.width = width;
	rect_transform_[resolution].local_rect.height = height;
	rect_transform_[resolution].local_rect.min = { max.x - width, max.y - height };
	rect_transform_[resolution].local_rect.center = { max.x - width / 2.0f, max.y - height / 2.0f };
}

void UIBase::ComputeLocalRectByCenter(E_Resolution resolution, XMFLOAT2 center, float width, float height)
{
	rect_transform_[resolution].local_rect.center = center;
	rect_transform_[resolution].local_rect.width = width;
	rect_transform_[resolution].local_rect.height = height;
	rect_transform_[resolution].local_rect.min = { center.x - width / 2.0f, center.y - height / 2.0f };
	rect_transform_[resolution].local_rect.max = { center.x + width / 2.0f, center.y + height / 2.0f };
}

void UIBase::ComputeLocalRectByMinMax(E_Resolution resolution, XMFLOAT2 min, XMFLOAT2 max)
{
	rect_transform_[resolution].local_rect.min = min;
	rect_transform_[resolution].local_rect.max = max;
	rect_transform_[resolution].local_rect.width = max.x - min.x;
	rect_transform_[resolution].local_rect.height = max.y - min.y;
	rect_transform_[resolution].local_rect.center =
	{ min.x + (rect_transform_[resolution].local_rect.width / 2.0f), min.y + (rect_transform_[resolution].local_rect.height / 2.0f) };
}


