#pragma once
#include "System.h"

namespace KGCA41B
{
	class DLL_API TransformSystem : public System
	{
	public:
		TransformSystem();
		~TransformSystem();

	public:
		virtual void OnCreate(entt::registry& reg);
		virtual void OnUpdate(entt::registry& reg);

	private:
		void SetCbTransform(Transform& transform);
	};
}

