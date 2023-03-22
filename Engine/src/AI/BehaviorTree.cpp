#include "stdafx.h"
#include "BehaviorTree.h"

namespace reality {
	void BehaviorTree::Update()
	{
		root_->Execute();
	}

	BehaviorStatus BehaviorTree::Execute()
	{
		return root_->Execute();
	}
}