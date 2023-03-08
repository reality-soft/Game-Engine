#include "stdafx.h"
#include "EventMgr.h"

void reality::EventMgr::PushEvent(const Event& event)
{
	event_queue_.push(make_shared<Event>(event));
}

void reality::EventMgr::ProcessEvents()
{
	while (!event_queue_.empty()) {
		Event* cur_event = event_queue_.front().get();

		cur_event->Process();

		event_queue_.pop();
	}
}