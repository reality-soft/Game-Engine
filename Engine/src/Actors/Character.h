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
		XMFLOAT3 floor_position = {0, 0, 0};
		MovementState movement_state_;
		vector<RayShape> blocking_walls_;

	public:
		void OnInit(entt::registry& registry) override;
		void OnUpdate() override;
		C_Movement* GetMovementComponent() { return movement_component_; }
		void GravityFall(float _gravity);
		XMMATRIX GetTranformMatrix() { return transform_matrix_; }

	public:
		XMVECTOR GetPos() const;
		void SetPos(const XMVECTOR& position);
		void RotateAlongMovementDirection();
	};
}