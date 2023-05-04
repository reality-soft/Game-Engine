#include "stdafx.h"
#include "Environment.h"
#include "TimeMgr.h"

bool reality::Environment::CreateEnvironment()
{
	if (sky_sphere_.CreateSphere() == false)
		return false;

	if (distance_fog_.CreateDistacneFog() == false)
		return false;

	return true;
}

void reality::Environment::SetWorldTime(float noon_time, float night_time)
{
	time_limits.x = noon_time / 2;
	time_limits.y = -night_time / 2;

	current_time_ = 0.0f;
	time_routin_ = TimeRoutin::NIGHT_TO_NOON;
	current_day_ = Day::eNoon;
}

void reality::Environment::SetSkyColorByTime(XMFLOAT4 start_color, XMFLOAT4 end_color)
{
	sky_sphere_.SetNoonColor(start_color);
	sky_sphere_.SetNightColor(end_color);
}

void reality::Environment::SetFogDistanceByTime(float start_distance, float end_distance)
{
	distance_fog_.SetMaxMinDistance(start_distance, end_distance);
}

void reality::Environment::SetLightProperty(XMFLOAT4 _noon_light_color, XMFLOAT4 _night_light_color, float min_spec, float max_spec)
{
	this->noon_light_color_ = _noon_light_color;
	this->night_light_color_ = _night_light_color;
	this->min_specular_ = min_spec;
	this->max_specular_ = max_spec;
}

void reality::Environment::Update(XMVECTOR camera_pos, LightingSystem* sys_lighting)
{
	switch (time_routin_)
	{
	case TimeRoutin::NOON_TO_NIGHT:
		current_time_ -= TM_DELTATIME;
		break;

	case TimeRoutin::NIGHT_TO_NOON:
		current_time_ += TM_DELTATIME;
		break;
	}

	if (current_time_ >= time_limits.x)
		time_routin_ = TimeRoutin::NOON_TO_NIGHT;


	else if (current_time_ <= time_limits.y)
		time_routin_ = TimeRoutin::NIGHT_TO_NOON;

	if (current_time_ > 0)
		current_day_ = Day::eNoon;

	else
		current_day_ = Day::eNight;

	float lerp_value = max(0, current_time_ / time_limits.x);

	sky_sphere_.Update(lerp_value);
	distance_fog_.Update(lerp_value);

	
	sys_lighting->UpdateGlobalLight(min_specular_ + (max_specular_ - min_specular_) * lerp_value, _XMFLOAT4(XMVectorLerp(_XMVECTOR4(night_light_color_), _XMVECTOR4(noon_light_color_), lerp_value)));
	distance_fog_.UpdateFogStart(camera_pos);
	distance_fog_.UpdateFogColor(sky_sphere_.GetSkyColor());

}

void reality::Environment::Render()
{
	DX11APP->GetDeviceContext()->OMSetBlendState(DX11APP->GetCommonStates()->Opaque(), 0, -1);
	sky_sphere_.Render();
	distance_fog_.Render();
}

reality::DistanceFog* reality::Environment::GetDistanceFog()
{
	return &distance_fog_;
}

reality::SkySphere* reality::Environment::GetSkySphere()
{
	return &sky_sphere_;
}

float reality::Environment::GetCountingTime()
{
	return current_time_;
}

XMFLOAT2 reality::Environment::GetTimeLimits()
{
	return time_limits;
}

bool reality::Environment::IsDayChanged()
{
	static Day last_day = current_day_;
	if (current_day_ != last_day)
	{
		last_day = current_day_;
		return true;
	}

	return false;
}

reality::Day reality::Environment::GetCurrentDay()
{
	return current_day_;
}
