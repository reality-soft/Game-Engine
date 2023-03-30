#pragma once
#include "Actor.h"

class Weapon : public reality::Actor
{
public:
	Weapon(entt::entity owner_id) : owner_id_(owner_id) {};

public:
	virtual void OnInit(entt::registry& registry);
	virtual void OnUpdate();

public:
	void		 SetOwnerId(entt::entity owner_id);
	void		 SetSocket(int socket_id);
	void		 SetOwnerTransform(XMMATRIX owner_transform);

protected:
	XMMATRIX		animation_matrix_ = XMMatrixIdentity();
	XMMATRIX		socket_offset_ = XMMatrixIdentity();
private:
	entt::entity owner_id_;
	int			 socket_id_ = -1;
	XMMATRIX	 owner_transform_ = XMMatrixIdentity();
};

