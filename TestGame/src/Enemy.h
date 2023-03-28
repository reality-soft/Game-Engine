#pragma once
#include "Engine_include.h"

class Enemy : public reality::Character
{
public:
	virtual void OnInit(entt::registry& registry) override;
	virtual void OnUpdate() override;
	void SetCharacterAnimation(string anim_id) const;
public:
	void Move();
	void Jump();
	void Idle();
	void Attack();
public:
	int	 GetMaxHp() const;
	int	 GetCurHp() const;
	XMVECTOR GetPos() const;
	void SetCurHp(int hp);
	void TakeDamage(int damage);
	void SetPos(const XMVECTOR& position);
	void SetDirection(const XMVECTOR& direction);
	void SetRoute(const vector<XMVECTOR>& target_poses);
	void SetMeshId(const string& mesh_id);
private:
	int max_hp_;
	int cur_hp_;
private:
	string mesh_id_;
	int cur_node = 0;
private:
	vector<XMVECTOR> target_poses_;
	int cur_target_pos_index_ = 1;
private:
	reality::BehaviorTree behavior_tree_;
};

class EnemyMoveToTargets : public reality::SequenceNode
{
public:
	EnemyMoveToTargets(entt::entity enemy_id, std::vector<XMVECTOR> target_positions)
		: target_positions_(target_positions), current_target_index_(0)
	{
		for (int i = 1;i < target_positions_.size();i++) 
		{
			auto target_pos = target_positions_[i];

		}
	}

private:
	std::vector<XMVECTOR> target_positions_;
	int current_target_index_;
};