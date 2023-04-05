#pragma once
#include "stdafx.h"

struct Transtion {
	int to_state_id;
	function<bool()> condition;
};