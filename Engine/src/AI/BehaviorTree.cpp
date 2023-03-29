#include "stdafx.h"
#include "BehaviorTree.h"

namespace reality {
	void BehaviorTree::Update()
	{
		if (root_ == nullptr) {
			return;
		}

		root_->Execute();

		BehaviorStatus root_status = root_->GetStatus();

		if (root_status == BehaviorStatus::FAILURE ||
			root_status == BehaviorStatus::SUCCESS) {
			root_->ResetNode();
		}
	}

	BehaviorNode* BehaviorTree::GetRootNode()
	{
		return root_.get();
	}
}