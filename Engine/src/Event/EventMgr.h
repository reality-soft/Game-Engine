#pragma once
#include "Events.h"

namespace reality {
	class DLL_API EventMgr
	{
		SINGLETON(EventMgr)
#define EVENT EventMgr::GetInst()
		queue<shared_ptr<Event>> event_queue_;

	public:
		void ProcessEvents();
	public:
		template <typename EventType, typename... Args>
		void PushEvent(Args&&...args)
		{
			event_queue_.push(make_shared<EventType>(args...));
		}
		void PushEvent(shared_ptr<Event> event)
		{
			event_queue_.push(event);
		}
	};
}


