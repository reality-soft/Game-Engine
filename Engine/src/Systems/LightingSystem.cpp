#include "stdafx.h"
#include "LightingSystem.h"
#include "TimeMgr.h"

using namespace reality;

LightingSystem::LightingSystem()
{
}

LightingSystem::~LightingSystem()
{
}

void LightingSystem::OnCreate(entt::registry& reg)
{
	

	if (FAILED(CreateSunCB()))
		return;

	if (FAILED(CreatePointLightsCB()))
		return; 
	
	if (FAILED(CreateSpotLightsCB()))
		return;
}

void LightingSystem::OnUpdate(entt::registry& reg)
{
	DX11APP->GetDeviceContext()->PSSetConstantBuffers(0, 1, global_light.buffer.GetAddressOf());

	UpdatePointLights(reg);

	UpdateSpotLights(reg);
}

HRESULT LightingSystem::CreateSunCB()
{
	HRESULT hr;

	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA subdata;

	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.ByteWidth = sizeof(CbGlobalLight::Data);

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	subdata.pSysMem = &global_light.data;

	hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, global_light.buffer.GetAddressOf());

	return hr;
}

HRESULT LightingSystem::CreatePointLightsCB()
{
	HRESULT hr;

	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA subdata;

	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.ByteWidth = sizeof(CbPointLight::Data);

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	subdata.pSysMem = &point_lights.data;

	hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, point_lights.buffer.GetAddressOf());

	return hr;
}

HRESULT LightingSystem::CreateSpotLightsCB()
{
	HRESULT hr;

	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA subdata;

	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.ByteWidth = sizeof(CbSpotLight::Data);

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	subdata.pSysMem = &spot_lights.data;

	hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, spot_lights.buffer.GetAddressOf());

	return hr;
}

void LightingSystem::UpdateSun(SkySphere& sky_shere)
{
	//XMStoreFloat4(&global_light.data.sun_position, sky_shere.sun_world.r[3]);
	//XMStoreFloat4(&global_light.data.direction, XMVector4Normalize(sky_shere.sun_world.r[3]));

	//DX11APP->GetDeviceContext()->UpdateSubresource(global_light.buffer.Get(), 0, 0, &global_light.data, 0, 0);
}


void LightingSystem::UpdatePointLights(entt::registry& reg)
{
	auto view = reg.view<C_PointLight>();
	int count = 0;
	for (auto& entity : view)
	{
		auto& point_light = reg.get<C_PointLight>(entity);
		// 1. Lifetime Check
		if (point_light.lifetime > 0 && point_light.lifetime < point_light.timer)
			continue;
		else
			point_light.timer += TIMER->GetDeltaTime();

		// 2. if Lifetime remain, Plus Data to CB Array
		CbPointLight::Data point_light_data;
		point_light_data.position = point_light.position;
		point_light_data.range = point_light.range;
		point_light_data.attenuation = point_light.attenuation;

		point_lights.data[count++] = point_light_data;
	}

	// 3. Update Constant Buffer & Set CB
	DX11APP->GetDeviceContext()->UpdateSubresource(point_lights.buffer.Get(), 0, 0, &point_lights.data, 0, 0);

	DX11APP->GetDeviceContext()->PSSetConstantBuffers(1, 1, point_lights.buffer.GetAddressOf());
}

void LightingSystem::UpdateSpotLights(entt::registry& reg)
{
	auto view = reg.view<C_SpotLight>();
	int count = 0;
	for (auto& entity : view)
	{
		auto& spot_light = reg.get<C_SpotLight>(entity);
		// 1. Lifetime Check
		if (spot_light.lifetime > 0 && spot_light.lifetime < spot_light.timer)
			continue;
		else
			spot_light.timer += TIMER->GetDeltaTime();

		// 2. if Lifetime remain, Plus Data to CB Array
		CbSpotLight::Data spot_light_data;
		spot_light_data.position = spot_light.position;
		spot_light_data.range = spot_light.range;
		spot_light_data.attenuation = spot_light.attenuation;
		spot_light_data.direction = spot_light.direction;
		spot_light_data.spot = spot_light.spot;

		spot_lights.data[count++] = spot_light_data;
	}

	// 3. Update Constant Buffer & Set CB
	DX11APP->GetDeviceContext()->UpdateSubresource(spot_lights.buffer.Get(), 0, 0, &point_lights.data, 0, 0);

	DX11APP->GetDeviceContext()->PSSetConstantBuffers(2, 1, spot_lights.buffer.GetAddressOf());
}
