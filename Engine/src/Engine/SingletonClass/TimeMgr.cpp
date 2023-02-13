#include "stdafx.h"
#include "TimeMgr.h"

using namespace KGCA41B;

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

	fps = 0;
	for (double i = 0; i < 1.0; i += delta_time)
	{
		fps++;
	}

	game_time += delta_time;
	last_time = cur_time;
}

double TimeMgr::GetDeltaTime()
{
	return delta_time;
}

float TimeMgr::GetFPS()
{
	return fps;
}

float TimeMgr::GetGameTime()
{
	return game_time;
}
