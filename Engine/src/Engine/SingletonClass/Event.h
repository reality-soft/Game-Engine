#pragma once
#include "DllMacro.h"

namespace reality {
	enum EVENT_TYPE {

	};

	class DLL_API Event
	{

		EVENT_TYPE event_type_;
		
	public:
		virtual void Process() {};
	};


}
