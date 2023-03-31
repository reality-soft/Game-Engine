#include "stdafx.h"
#include "LightingSystem.h"
#include "TimeMgr.h"
#include "SceneMgr.h"

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

	desc.ByteWidth = sizeof(CbPointLight::Data) * 64;

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	subdata.pSysMem = point_lights.data;

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

	desc.ByteWidth = sizeof(CbSpotLight::Data) * 64;

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	subdata.pSysMem = spot_lights.data;

	hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, spot_lights.buffer.GetAddressOf());

	return hr;
}

void LightingSystem::UpdateGlobalLight(XMFLOAT2 world_time, float current_time, float min_bright, float max_specular)
{
	float half_noon = world_time.x / 2;
	float half_night = world_time.y / 2;

	if (current_time > half_noon)
	{
		global_light.data.brightness = 1.0f;
		global_light.data.specular_strength = max_specular;
	}

	else if (current_time < half_night)
	{
		global_light.data.brightness = min_bright;
		global_light.data.specular_strength = 0.0f;
	}

	else if (current_time <= half_noon && current_time >= half_night)
	{
		float a = half_noon - half_night;
		float b = half_night - half_night;
		float ct = current_time - half_night;
		float lerp = ct / (a + b);

		global_light.data.brightness = max(min_bright, lerp);
		global_light.data.specular_strength = lerp * max_specular;
	}

	DX11APP->GetDeviceContext()->UpdateSubresource(global_light.buffer.Get(), 0, 0, &global_light.data, 0, 0);
	DX11APP->GetDeviceContext()->PSSetConstantBuffers(0, 1, global_light.buffer.GetAddressOf());

}

CbGlobalLight::Data reality::LightingSystem::GetGlobalLightData()
{
	return global_light.data;
}

void reality::LightingSystem::SetGlobalLightPos(XMFLOAT3 position)
{
	global_light.data.position = position;
}

void LightingSystem::UpdatePointLights(entt::registry& reg)
{
	// 0. Clear the last frame PointLights CBData
	ZeroMemory(point_lights.data, sizeof(CbPointLight::Data) * 64);

	auto view = reg.view<C_PointLight>();
	int count = 0;
	for (auto& entity : view)
	{
		auto& point_light_comp = reg.get<C_PointLight>(entity);

		// 1. Lifetime Check
		if (point_light_comp.lifetime > 0 && point_light_comp.lifetime < point_light_comp.timer)
			continue;
		else
			point_light_comp.timer += TIMER->GetDeltaTime();

		// 2. if Lifetime remain, Plus Data to CB Array
		CbPointLight::Data point_light_data;

		XMVECTOR S, R, T;
		XMMatrixDecompose(&S, &R, &T, point_light_comp.world);

		XMStoreFloat3(&point_light_data.position, T);

		point_light_data.light_color = point_light_comp.point_light.light_color;
		point_light_data.range = point_light_comp.point_light.range;
		point_light_data.attenuation_level = point_light_comp.point_light.attenuation_level;
		point_light_data.attenuation = point_light_comp.point_light.attenuation;
		point_light_data.specular = point_light_comp.point_light.specular;

		point_lights.data[count++] = point_light_data;
	}

	// 3. Update Constant Buffer & Set CB
	DX11APP->GetDeviceContext()->UpdateSubresource(point_lights.buffer.Get(), 0, 0, point_lights.data, 0, 0);

	DX11APP->GetDeviceContext()->PSSetConstantBuffers(2, 1, point_lights.buffer.GetAddressOf());
}

void LightingSystem::UpdateSpotLights(entt::registry& reg)
{
	// 0. Clear the last frame SpotLights CBData
	ZeroMemory(spot_lights.data, sizeof(CbSpotLight::Data) * 64);

	auto view = reg.view<C_SpotLight>();
	int count = 0;
	for (auto& entity : view)
	{
		auto& spot_light_comp = reg.get<C_SpotLight>(entity);

		// 1. Lifetime Check
		if (spot_light_comp.lifetime > 0 && spot_light_comp.lifetime < spot_light_comp.timer)
			continue;
		else
			spot_light_comp.timer += TIMER->GetDeltaTime();

		// 2. if Lifetime remain, Plus Data to CB Array
		CbSpotLight::Data spot_light_data;

		XMVECTOR S, R, T;
		XMMatrixDecompose(&S, &R, &T, spot_light_comp.world);

		XMStoreFloat3(&spot_light_data.position, T);
		XMVECTOR direction_vec = XMVector3Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMMatrixRotationQuaternion(R));
		XMStoreFloat3(&spot_light_data.direction, direction_vec);

		spot_light_data.light_color = spot_light_comp.spot_light.light_color;

		spot_light_data.range = spot_light_comp.spot_light.range;
		spot_light_data.attenuation_level = spot_light_comp.spot_light.attenuation_level;
		spot_light_data.attenuation = spot_light_comp.spot_light.attenuation;
		spot_light_data.specular = spot_light_comp.spot_light.specular;
		spot_light_data.spot = spot_light_comp.spot_light.spot;

		spot_lights.data[count++] = spot_light_data;
	}

	// 3. Update Constant Buffer & Set CB
	DX11APP->GetDeviceContext()->UpdateSubresource(spot_lights.buffer.Get(), 0, 0, spot_lights.data, 0, 0);

	DX11APP->GetDeviceContext()->PSSetConstantBuffers(3, 1, spot_lights.buffer.GetAddressOf());
}
