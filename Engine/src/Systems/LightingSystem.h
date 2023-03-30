#pragma once
#include "System.h"
#include "SkySphere.h"

namespace reality
{
	class DLL_API LightingSystem : public System
	{
	public:
		LightingSystem();
		~LightingSystem();
	public:	
		void OnCreate(entt::registry& reg) override;
		void OnUpdate(entt::registry& reg) override;
		void UpdateGlobalLight(XMFLOAT2 world_time, float current_time, float min_bright, float max_specular);

	private:
		HRESULT CreateSunCB();
		HRESULT CreatePointLightsCB();
		HRESULT CreateSpotLightsCB();

		void UpdatePointLights(entt::registry& reg);
		void UpdateSpotLights(entt::registry& reg);

		CbGlobalLight global_light;
		CbPointLight point_lights;
		CbSpotLight spot_lights;
	};
}
