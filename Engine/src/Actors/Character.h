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
		C_Movement*	 movement_component_;
		XMVECTOR	 front_ = { 0, 0, 1, 0 };
		XMVECTOR	 right_ = { 1, 0, 0, 0 };
		XMVECTOR	 fall_ = { 0, -1, 0, 0 };
		XMVECTOR	 jump_ =  { 0, 1, 0, 0 };

	protected:
		XMMATRIX rotation_ = XMMatrixIdentity();

	public:
		XMFLOAT3 floor_position = {0, 0, 0};
		MovementState movement_state_;
		XMFLOAT4 blocking_planes_[4];

	public:
		void OnInit(entt::registry& registry) override;
		void OnUpdate() override;

	public:
		C_Movement* GetMovementComponent() { return movement_component_; }
		void GravityFall(float _gravity);		
		void SetPos(const XMVECTOR& position);
		XMMATRIX GetRotation();

	public:
		void CancelMovement();
	};
}