#include "stdafx.h"
#include "UISystem.h"
#include "Engine.h"
#include "SceneMgr.h"

using namespace reality;

CbUI UISystem::cb_UI = {};

void UISystem::OnCreate(entt::registry& reg)
{
	render_target_ = RENDER_TARGET->MakeRT("UI", ENGINE->GetWindowSize().x, ENGINE->GetWindowSize().y);
	render_target_->SetClearColor({ 0, 0, 0, 0 });

	HRESULT hr;
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA subdata;

	// Init UI Constant Buffer
	{
		ZeroMemory(&cb_UI.data, sizeof(CbUI::Data));

		ZeroMemory(&desc, sizeof(desc));
		ZeroMemory(&subdata, sizeof(subdata));

		cb_UI.data.world = XMMatrixIdentity();

		desc.ByteWidth = sizeof(CbUI::Data);

		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		subdata.pSysMem = &cb_UI.data;

		hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, cb_UI.buffer.GetAddressOf());
	}
	
}

void UISystem::OnUpdate(entt::registry& reg)
{
	// UI 랜더타겟으로 변경 후 UI들 랜더링
	render_target_->SetRenderTarget();
	
	auto view_ui = reg.view<C_UI>();
	for (auto& entity : view_ui)
	{
		C_UI& ui_comp = reg.get<C_UI>(entity);
		for (auto& pair : ui_comp.ui_list)
		{
			if (!pair.second->GetOnOff())
				continue;
			pair.second->Update();
		}

		for (auto& pair : ui_comp.ui_list)
		{
			pair.second->Render();
		}
	}

	// 백버퍼에 UI 랜더타겟 랜더링
	float scale_x = render_target_->rect_.width / ENGINE->GetWindowSize().x;
	float scale_y = render_target_->rect_.height / ENGINE->GetWindowSize().y;
	float pos_x = render_target_->rect_.center.x / ENGINE->GetWindowSize().x;
	float pos_y = render_target_->rect_.center.y / ENGINE->GetWindowSize().y;

	
	XMMATRIX s = XMMatrixScaling(scale_x, scale_y, 1.0f);
	XMMATRIX r = XMMatrixRotationZ(0.0f);
	XMMATRIX t = XMMatrixTranslation(pos_x, pos_y, 0.0f);
	SetCbData(XMMatrixTranspose(s * r * t));
	render_target_->RenderingRT();

}


void UISystem::SetCbData(XMMATRIX world)
{
	cb_UI.data.world = world;
	DX11APP->GetDeviceContext()->UpdateSubresource(cb_UI.buffer.Get(), 0, nullptr, &cb_UI.data, 0, 0);
	DX11APP->GetDeviceContext()->VSSetConstantBuffers(1, 1, cb_UI.buffer.GetAddressOf());
}

