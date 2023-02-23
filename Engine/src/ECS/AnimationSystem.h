#pragma once
#include "System.h"

namespace KGCA41B
{
	class DLL_API AnimationSystem : public System
	{
	public:
		AnimationSystem();
		~AnimationSystem();

	public:
		virtual void OnCreate(entt::registry& reg);
		virtual void OnUpdate(entt::registry& reg);

	private:
		void PlayAnimation(Skeleton& skeleton, C_Animation& animation);

		ID3D11DeviceContext* device_context = nullptr;
	};
}
