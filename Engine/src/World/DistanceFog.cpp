#include "stdafx.h"
#include "DistanceFog.h"
#include "DX11App.h"

bool reality::DistanceFog::CreateDistacneFog()
{
	HRESULT hr;

	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA subresource;
	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subresource, sizeof(subresource));

	desc.ByteWidth = sizeof(CbFog::Data);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	subresource.pSysMem = &cb_fog_.data;

	hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subresource, cb_fog_.buffer.GetAddressOf());
	if (FAILED(hr))
		return false;

	DX11APP->GetDeviceContext()->UpdateSubresource(cb_fog_.buffer.Get(), 0, 0, &cb_fog_.data, 0, 0);
	DX11APP->GetDeviceContext()->PSSetConstantBuffers(4, 1, cb_fog_.buffer.GetAddressOf());

	return true;
}

void reality::DistanceFog::SetMaxMinDistance(float max, float min)
{
	max_distance_ = max;
	min_distance_ = min;
}

void reality::DistanceFog::UpdateFogStart(XMVECTOR fog_start)
{
	cb_fog_.data.fog_start = XMFLOAT3(fog_start.m128_f32);
}

void reality::DistanceFog::UpdateFogColor(XMFLOAT4 color)
{
	cb_fog_.data.fog_color = color;
}

void reality::DistanceFog::Update(float lerp_value)
{
	cb_fog_.data.distance = min_distance_ + (max_distance_ - min_distance_) * lerp_value;
}

void reality::DistanceFog::Render()
{
	DX11APP->GetDeviceContext()->UpdateSubresource(cb_fog_.buffer.Get(), 0, 0, &cb_fog_.data, 0, 0);
	DX11APP->GetDeviceContext()->PSSetConstantBuffers(4, 1, cb_fog_.buffer.GetAddressOf());
}
