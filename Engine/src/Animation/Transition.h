#pragma once
#include "stdafx.h"

namespace reality {
	struct Transition {
		int to_state_id;
		function<bool()> condition;
	};
}