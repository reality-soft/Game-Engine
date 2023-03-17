#pragma once
#include "Actor.h"

namespace reality
{
	class DLL_API Character : public Actor
	{
	protected:
		C_Movement*	 movement_component_;
		XMVECTOR	 front_ = { 0, 0, 1, 0 };
		XMVECTOR	 right_ = { 1, 0, 0, 0 };
		XMVECTOR	 fall_ = { 0, -1, 0, 0 };
		XMVECTOR	 jump_ =  { 0, 1, 0, 0 };

	public:
		CapsuleCallback capsule_callback;
		void		 OnInit(entt::registry& registry) override;
		void ApplyMovement(XMMATRIX movement_matrix) override;
		C_Movement* GetMovementComponent() { return movement_component_; };
		void GravityFall(float _gravity);

	protected:
		void FastenAtFloor(XMMATRIX& movement_matrix);
	};
}