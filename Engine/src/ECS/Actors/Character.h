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
	public:
		void		 OnInit(entt::registry& registry) override;
	};
}