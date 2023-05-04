#pragma once
#include "Animation.h"

namespace reality {

	class Event;

	enum class ANIM_STATE {
		ANIM_STATE_NONE,
		ANIM_STATE_CUR_ONLY,
		ANIM_STATE_PREV_ONLY,
		ANIM_STATE_CUR_PREV,
	};

	class DLL_API AnimationBase {
	public:
		AnimationBase(string skeletal_mesh_id, int range, string bone_name = "");

	public:
		Animation GetCurAnimation() {
			return animation_;
		}

	public:
		virtual void AnimationUpdate();
		virtual void OnInit() {};

	protected:
		virtual void OnUpdate() {};

	public:
		bool IsAnimationEnded() {
			return animation_ended_;
		}

	public:
		string GetCurAnimationId();
		float GetCurAnimTime();
		float GetCurFrame();
		float GetBlendTime();
		float GetRange();

	public:
		unordered_map<UINT, XMMATRIX>* GetAnimationMatrices();
		unordered_map<UINT, XMMATRIX>* GetPrevAnimationMatrices();
		unordered_map<UINT, int>* GetWeights();

	public:
		ANIM_STATE GetCurAnimState();

	public:
		virtual void SetAnimation(string animation_id, float blend_time, bool loop, vector<AnimNotify> notifies = vector<AnimNotify>());

	protected:
		Animation animation_;
		ANIM_STATE cur_anim_state_ = ANIM_STATE::ANIM_STATE_NONE;
		bool animation_ended_ = false;
	};
}

