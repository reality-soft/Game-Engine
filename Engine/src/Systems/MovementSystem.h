#pragma once
#include "System.h"

namespace reality
{
	class DLL_API MovementSystem : public System
	{
	public:
		MovementSystem() = default;
		~MovementSystem() = default;

		virtual void OnCreate(entt::registry& reg) override;
		virtual void OnUpdate(entt::registry& reg) override;

		XMFLOAT2 ndc;
	private:



	private:

	};
}
