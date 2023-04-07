#pragma once
#include "stdafx.h"
#include "AnimationBase.h"

namespace reality {
	struct AnimSlot {
	public:
		map<int, unordered_set<UINT>> included_skeletons_;
		unordered_map<UINT, int>	  bone_id_to_weight_;
		shared_ptr<AnimationBase>	  anim_object_;
		float						  range_;	
	public:
		void OnUpdate() const {
			anim_object_->AnimationUpdate();
		};
		bool IsValid() const {
			return anim_object_->IsValid();
		}
	};
}
