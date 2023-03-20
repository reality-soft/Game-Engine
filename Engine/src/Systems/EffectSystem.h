#pragma once
#include "System.h"
#include "Effect.h"

namespace reality
{
	class DLL_API EffectSystem : public System
	{
	public:
		virtual void OnCreate(entt::registry& reg);
		virtual void OnUpdate(entt::registry& reg);
	public:
		void UpdateParticles(Emitter* emitter);
		void EmitParticles(Emitter* emitter);
		void EmitParticle(Emitter* emitter);
	};
}
