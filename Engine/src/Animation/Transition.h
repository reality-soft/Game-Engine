#pragma once
#include "stdafx.h"

struct Transition {
	int to_state_id;
	function<bool()> condition;
};