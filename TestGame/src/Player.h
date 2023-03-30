#pragma once
#include "Engine_include.h"

class Player : public reality::Character
{
public:
	virtual void OnInit(entt::registry& registry) override;
	virtual void OnUpdate() override;
	void SetCharacterAnimation(string anim_id);
public:
	void MoveRight();
	void MoveRightForward();
	void MoveRightBack();
	void MoveLeft();
	void MoveLeftForward();
	void MoveLeftBack();
	void MoveForward();
	void MoveBack();
	void Jump();
	void Idle();
	void Fire();
	void ResetPos();	
public:
	int GetMaxHp() const;
	void SetCurHp(int hp);
	void TakeDamage(int damage);
	int GetCurHp() const;
private:
	int max_hp_;
	int cur_hp_;
};

