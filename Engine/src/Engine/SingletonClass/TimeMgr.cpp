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
	BOOL isUpdated = QueryPerformanceCounter((LARGE_INTEGER*)&cur_time);
	delta_time = ((double)cur_time.QuadPart - (double)last_time.QuadPart) / (double)period_frequency.QuadPart;

	static float frames = 0.0f;
	static double oneSec = 0.0f;

	frames += 1.0f;
	oneSec += delta_time;
	game_time += delta_time;
	if (oneSec >= 1.0f)
	{
		fps = frames;

		frames = 0;
		oneSec = 0;
	}

	last_time = cur_time;
}

double TimeMgr::GetDeltaTime()
{
	return delta_time;
}

int TimeMgr::GetFPS()
{
	return (int)fps;
}

float TimeMgr::GetGameTime()
{
	return game_time;
}
