#pragma once
#include "System.h"

namespace KGCA41B
{
	struct CbLight
	{
		CbLight()
		{
			data.direction = { 0, -1, 0, 0 };
			data.color = {1, 1, 1, 1};
		}
		CbLight(const CbLight& other)
		{
			data = other.data;
			other.buffer.CopyTo(buffer.GetAddressOf());
		}
		struct Data
		{
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

		CbLight global_light;
	};
}
