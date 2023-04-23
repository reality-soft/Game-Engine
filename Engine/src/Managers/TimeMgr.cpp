#include "stdafx.h"
#include "TimeMgr.h"

using namespace reality;

void TimeMgr::Init()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&period_frequency);
	QueryPerformanceCounter((LARGE_INTEGER*)&last_time);
	delta_time = 0;
	fps = 0;
	game_time = 0;
}

void TimeMgr::Update()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&cur_time);
	delta_time = ((double)cur_time.QuadPart - (double)last_time.QuadPart) / (double)period_frequency.QuadPart;

	static int fps_ = 0;
	if (deltas < 1.0f)
	{
		deltas += delta_time;
		fps_++;
	}
	else
	{
		deltas = 0.0f;
		fps = fps_;
		fps_ = 0;
	}
	//fps = 0;
	//for (double i = 0; i < 1.0; i += delta_time)
	//{
	//	fps++;
	//}

	game_time += delta_time;
	last_time = cur_time;
}

double TimeMgr::GetDeltaTime()
{
	return delta_time;
}

int TimeMgr::GetFPS()
{
	return fps;
}

float TimeMgr::GetGameTime()
{
	return game_time;
}
