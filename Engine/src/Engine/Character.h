#pragma once
#include "Actor.h"

namespace KGCA41B
{
	class DLL_API Character : public Actor
	{
	protected:
		C_Movement*	 movement_component_;
	public:
		void		 CharacterInit(entt::registry& registry, AABBShape collision_box);
	};
}