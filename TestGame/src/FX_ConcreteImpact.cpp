#include "stdafx.h"
#include "FX_ConcreteImpact.h"
#include "SceneMgr.h"
#include "ResourceMgr.h"

using namespace reality;

void FX_ConcreteImpact::OnInit(entt::registry& registry)
{
	FX_BaseEffectActor::OnInit(registry);

	auto& effect_comp = registry.get<C_Effect>(GetEntityId());
	effect_comp.effect_id = "concrete_effect";
	effect_comp.effect = *RESOURCE->UseResource<Effect>(effect_comp.effect_id);
}

void FX_ConcreteImpact::OnUpdate()
{
	FX_BaseEffectActor::OnUpdate();
}
