#pragma once
#include "Actor.h"

namespace reality
{
	enum class MovementState
	{
		STAND_ON_FLOOR,
		JUMP,
		GRAVITY_FALL,
		BLOCK_BY_WALL,
	};

	class DLL_API Character : public Actor
	{
	protected:
		C_Movement*	 movement_component_;
		XMVECTOR	 front_ = { 0, 0, 1, 0 };
		XMVECTOR	 right_ = { 1, 0, 0, 0 };
		XMVECTOR	 fall_ = { 0, -1, 0, 0 };
		XMVECTOR	 jump_ =  { 0, 1, 0, 0 };

	public:
		float floor_height = 0.0f;
		MovementState movement_state_;
		CapsuleCallback capsule_callback;

	public:
		void		OnInit(entt::registry& registry) override;
		void		ApplyMovement(XMMATRIX movement_matrix) override;
		void    OnUpdate() override;

  public:
		C_Movement* GetMovementComponent() { return movement_component_; }
		void		GravityFall(float _gravity);
		XMMATRIX	transform_matrix() { return transform_matrix_; }

	protected:
		void		FastenAtFloor();
	};
}