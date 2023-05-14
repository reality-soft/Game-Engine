#pragma once
#include "Actor.h"

namespace reality
{
	enum class MovementState
	{
		FALL,
		WALK,
	};

	class DLL_API Character : public Actor
	{
	protected:
		XMVECTOR	 front_ = { 0, 0, 1, 0 };
		XMVECTOR	 right_ = { 1, 0, 0, 0 };
		XMVECTOR	 fall_ = { 0, -1, 0, 0 };
		XMVECTOR	 jump_ =  { 0, 1, 0, 0 };

	public:
		XMMATRIX rotation_ = XMMatrixIdentity();

	public:
		XMFLOAT3 floor_position = {0, 0, 0};
		MovementState movement_state_;
		XMFLOAT4 blocking_planes_[4];

	public:
		void OnInit(entt::registry& registry) override;
		void OnUpdate() override;

	public:
		C_Movement* GetMovementComponent();
		C_CapsuleCollision* GetCapsuleComponent();

		void SetPos(const XMVECTOR& position);
		XMMATRIX GetRotation();
		XMVECTOR GetFront() { return front_; };
	public:
		void CancelMovement();
		bool rotate_enable_ = true;
	};
}