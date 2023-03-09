#pragma once
#include "Actor.h"

namespace reality
{
	class DLL_API Character : public Actor
	{
	protected:
		C_Movement*	 movement_component_;
	public:
		void		 OnInit(entt::registry& registry) override;
	};
}