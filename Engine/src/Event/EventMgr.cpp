#include "stdafx.h"
#include "EventMgr.h"

void reality::EventMgr::ProcessEvents()
{
	while (!event_queue_.empty()) {
		Event* cur_event = event_queue_.front().get();

		cur_event->Process();

		event_queue_.pop();
	}
}