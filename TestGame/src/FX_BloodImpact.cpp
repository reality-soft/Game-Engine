#include "stdafx.h"
#include "FX_BloodImpact.h"
#include "SceneMgr.h"
#include "ResourceMgr.h"

using namespace reality;

void FX_BloodImpact::OnInit(entt::registry& registry)
{
	FX_BaseEffectActor::OnInit(registry);

	auto& effect_comp = registry.get<C_Effect>(GetEntityId());
	effect_comp.effect_id = "blood_effect";
	effect_comp.effect = *RESOURCE->UseResource<Effect>(effect_comp.effect_id);
}

void FX_BloodImpact::OnUpdate()
{
	FX_BaseEffectActor::OnUpdate();
}
