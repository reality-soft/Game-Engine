#pragma once
#include "stdafx.h"
#include "Event.h"

namespace reality {
	class DLL_API EventMgr
	{
		SINGLETON(EventMgr)
#define EVENT EventMgr::GetInst()
		queue<shared_ptr<Event>> event_queue_;

	public:
		void PushEvent(const Event& event);
		void ProcessEvents();
	};
}


