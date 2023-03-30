#pragma once
#include "CameraSystem.h"
#include "LightingSystem.h"
#include "SkySphere.h"
#include "DistanceFog.h"

class CameraSystem;

namespace reality
{
	enum class TimeRoutin
	{
		NOON_TO_NIGHT,
		NIGHT_TO_NOON
	};

	class DLL_API Environment
	{
	public:
		bool CreateEnvironment();
		void SetWorldTime(float noon_time, float night_time, bool noon_start);
		void SetSkyColorByTime(XMFLOAT4 start_color, XMFLOAT4 end_color);
		void SetFogDistanceByTime(float start_distance, float end_distance);
		void SetLightProperty(float _min_brightness, float _max_specular);

		void Update(CameraSystem* sys_camera, LightingSystem* sys_lighting);
		void Render();

	public:
		DistanceFog* GetDistanceFog();
		SkySphere* GetSkySphere();


	private:
		TimeRoutin time_routin_;
		XMFLOAT2 world_time_;
		float current_time_;
		float min_directional_bright_;
		float max_specular_strength_;

		DistanceFog distance_fog_;
		SkySphere   sky_sphere_;
	};
}
