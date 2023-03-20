#pragma once
#include "Actor.h"

class Weapon : public reality::Actor
{
public:
	Weapon(entt::entity owner) : owner_(owner) {};

public:
	virtual void OnInit(entt::registry& registry);
	virtual void OnUpdate();

public:
	void		 SetOwnerId(entt::entity owner_id);
	void		 SetSocket(int socket_id);

protected:
	XMMATRIX		animation_matrix_ = XMMatrixIdentity();
	XMMATRIX		socket_offset_ = XMMatrixIdentity();
private:
	entt::entity owner_;
	int			 socket_id_ = -1;
};

