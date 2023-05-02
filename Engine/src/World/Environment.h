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
		void SetLightProperty(XMFLOAT4 _noon_light_color, XMFLOAT4 _night_light_color, float min_spec, float max_spec);

		void Update(XMVECTOR camera_pos, LightingSystem* sys_lighting);
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

		// Light Property
		XMFLOAT4 noon_light_color_;
		XMFLOAT4 night_light_color_;
		float min_specular_;
		float max_specular_;

		DistanceFog distance_fog_;
		SkySphere   sky_sphere_;
	};
}
