#pragma once
#include "stdafx.h"

namespace reality {

	class Event;

	struct DLL_API AnimNotify {
		int frame;
		std::shared_ptr<Event> event;
		bool is_managed = false;
	};
}