#pragma once
#include "Actor.h"

namespace reality
{
	class DLL_API SpotLightActor : public Actor
	{
	public:
		virtual void OnInit(entt::registry& registry) override;
		virtual void OnUpdate() override;
	public:
		void Spawn(XMVECTOR pos, XMVECTOR rotation_q = XMQuaternionIdentity(), XMVECTOR scale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
		void Spawn(XMVECTOR pos, float lifetime, XMVECTOR rotation_q = XMQuaternionIdentity(), XMVECTOR scale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
	};
}


