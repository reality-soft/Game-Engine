#pragma once
#include "System.h"
#include "SkySphere.h"
namespace reality
{
	struct CbLight
	{
		CbLight()
		{
			data.direction = { 0, -1, 0, 0 };
			data.color = {1.5, 1.5, 1.5, 1.5};
		}
		CbLight(const CbLight& other)
		{
			data = other.data;
			other.buffer.CopyTo(buffer.GetAddressOf());
		}
		struct Data
		{
			XMFLOAT4 sun_position;
			XMFLOAT4 direction;
			XMFLOAT4 color;
		} data;

		ComPtr<ID3D11Buffer> buffer;
	};

	class DLL_API LightingSystem : public System
	{
	public:
		LightingSystem();
		~LightingSystem();
	public:	
		void OnCreate(entt::registry& reg) override;
		void OnUpdate(entt::registry& reg) override;
		void UpdateSun(SkySphere& sky_shere);

		CbLight global_light;
	};
}
