#include "stdafx.h"
#include "UISystem.h"
#include "Engine.h"

using namespace reality;

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

		desc.ByteWidth = sizeof(CbEffect::Data);

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
			pair.second->Update();
		}

		for (auto& pair : ui_comp.ui_list)
		{
			cb_UI.data.world = XMMatrixTranspose(pair.second->ui_transform_.world);
			SetCbData();
			pair.second->Render();
		}
	}

	// 백버퍼에 UI 랜더타겟 랜더링
	cb_UI.data.world = XMMatrixIdentity();
	SetCbData();
	render_target_->RenderingRT();
}

void UISystem::SetCbData()
{
	DX11APP->GetDeviceContext()->UpdateSubresource(cb_UI.buffer.Get(), 0, nullptr, &cb_UI.data, 0, 0);
	DX11APP->GetDeviceContext()->VSSetConstantBuffers(1, 1, cb_UI.buffer.GetAddressOf());
}

