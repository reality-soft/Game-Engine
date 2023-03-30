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

void reality::Environment::SetWorldTime(float noon_time, float night_time, bool noon_start)
{
	world_time_.x = noon_time;
	world_time_.y = -night_time;

	if (noon_start)
	{
		current_time_ = world_time_.x; 
		time_routin_ = TimeRoutin::NOON_TO_NIGHT;
	}

	else
	{
		current_time_ = -world_time_.y; 
		time_routin_ = TimeRoutin::NIGHT_TO_NOON;
	}
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

void reality::Environment::Update(CameraSystem* sys_camera)
{
	if (current_time_ >= world_time_.x)
		time_routin_ = TimeRoutin::NOON_TO_NIGHT;


	if (current_time_ <= world_time_.y)
		time_routin_ = TimeRoutin::NIGHT_TO_NOON;


	switch (time_routin_)
	{
	case TimeRoutin::NOON_TO_NIGHT:
		current_time_ -= TM_DELTATIME;
		break;

	case TimeRoutin::NIGHT_TO_NOON:
		current_time_ += TM_DELTATIME;
		break;
	}

	sky_sphere_.Update(world_time_, current_time_);
	distance_fog_.Update(world_time_, current_time_);

	distance_fog_.UpdateFogStart(sys_camera->GetCamera()->camera_pos);
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
