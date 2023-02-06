#pragma once
#include "common.h"
#include "DllMacro.h"
#include "ComponentSystem.h"

namespace KGCA41B
{
	class DLL_API Actor
	{
	protected:
		entt::entity ent;
		shared_ptr<Actor> parent_;

	public:
		void Regist(entt::registry& reg);

	public:
		Transform comp_transform_;
		void InheritTransform();
	};
}

