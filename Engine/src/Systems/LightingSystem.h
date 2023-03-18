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
		void UpdateSun(SkySphere& sky_shere);

		CbLight global_light;
	};
}
