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

	enum class Day
	{
		eNoon,
		eNight,
		NONE,
	};

	class DLL_API Environment
	{
	public:
		bool CreateEnvironment();
		void SetWorldTime(float noon_time, float night_time);
		void SetSkyColorByTime(XMFLOAT4 start_color, XMFLOAT4 end_color);
		void SetFogDistanceByTime(float start_distance, float end_distance);
		void SetLightProperty(float _min_brightness, float _max_specular);

		void Update(CameraSystem* sys_camera, LightingSystem* sys_lighting);
		void Render();

	public:
		DistanceFog* GetDistanceFog();
		SkySphere* GetSkySphere();
		float GetCountingTime();
		XMFLOAT2 GetTimeLimits();
		bool IsDayChanged();
		Day GetCurrentDay();

	private:
		TimeRoutin time_routin_;
		Day current_day_;

		float current_time_;
		XMFLOAT2 time_limits;
		float min_directional_bright_;
		float max_specular_strength_;

		DistanceFog distance_fog_;
		SkySphere   sky_sphere_;
	};
}
