#pragma once
#include "stdafx.h"
#include "DllMacro.h"

namespace KGCA41B
{
	class DLL_API TimeMgr
	{
		SINGLETON(TimeMgr);

	public:
		LARGE_INTEGER period_frequency;
		LARGE_INTEGER last_time;
		LARGE_INTEGER cur_time;

		double delta_time = 0;
		float fps = 0;
		float game_time = 0;

		void Init();
		void Update();

		double GetDeltaTime();
		float GetFPS();
		float GetGameTime();
	};
}
#define TIMER        KGCA41B::TimeMgr::GetInst()
#define TM_GAMETIME  KGCA41B::TimeMgr::GetInst()->GetGameTime()
#define TM_DELTATIME KGCA41B::TimeMgr::GetInst()->GetDeltaTime()
#define TM_FPS       KGCA41B::TimeMgr::GetInst()->GetFPS()

