#pragma once
#include "Engine_include.h"

class Enemy : public reality::Character
{
public:
	virtual void OnInit(entt::registry& registry) override;
	virtual void OnUpdate() override;
	void SetCharacterAnimation(string anim_id);
public:
	void Move();
	void Jump();
	void Idle();
	void Attack();
public:
	int	 GetMaxHp() const;
	int	 GetCurHp() const;
	void SetCurHp(int hp);
	void TakeDamage(int damage);
	void SetPos(const XMVECTOR& position);
	void SetGuideLines(const vector<reality::GuideLine> guide_lines);
private:
	int max_hp_;
	int cur_hp_;
private:
	string mesh_id_;
	int cur_node = 0;
private:
	vector<reality::GuideLine> guide_lines_;
private:
	reality::BehaviorTree behavior_tree_;
};

class MoveToTargetSequence : reality::SequenceNode {
	MoveToTargetSequence() {

	}

};

class MoveToPoint : reality::ActionNode {


};