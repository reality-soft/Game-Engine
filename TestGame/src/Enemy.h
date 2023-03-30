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
	void SetCurHp(int hp);
	void TakeDamage(int damage);
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
	reality::BehaviorTree behavior_tree_;
};

class EnemyMoveToTargets : public reality::ActionNode
{
public:
	EnemyMoveToTargets(entt::entity enemy_id, XMVECTOR target_position)
		: enemy_id_(enemy_id), target_position_(target_position)
	{
	};

	virtual reality::BehaviorStatus Action() override
	{
		Enemy* enemy = reality::SCENE_MGR->GetActor<Enemy>(enemy_id_);

		enemy->SetCharacterAnimation("Zombie_Walk_F_6_Loop_IPC_Anim_Unreal Take.anim");

		XMVECTOR cur_pos = enemy->GetPos();
		enemy->SetDirection(XMVector3Normalize(target_position_ - cur_pos));
		enemy->RotateAlongMovementDirection();

		if (XMVector3Length(target_position_ - cur_pos).m128_f32[0] < 100.0f) {
			return reality::BehaviorStatus::SUCCESS;
		}

		return reality::BehaviorStatus::RUNNING;
	}

private:
	XMVECTOR target_position_;
	entt::entity enemy_id_;
	std::mutex  movement_mutex_;
};