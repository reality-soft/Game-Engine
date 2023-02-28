#pragma once
#include "System.h"

namespace KGCA41B
{
	class DLL_API EffectSystem : public System
	{
	public:
		virtual void OnCreate(entt::registry& reg);
		virtual void OnUpdate(entt::registry& reg);
	public:
		void UpdateTimer(Emitter* emitter);
		void EmitParticles(Emitter* emitter);
		void EmitParticle(Emitter* emitter);
	};
}
