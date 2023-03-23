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

	auto& sound_gen_comp = registry.get<C_SoundGenerator>(GetEntityId());
	SoundQueue sound_queue;
	sound_queue.sound_filename = "S_WEP_Fire_001.wav";
	sound_queue.is_looping = false;
	sound_queue.sound_type = SoundType::SFX;
	sound_queue.sound_volume = 10.0f;
	sound_gen_comp.sound_queue_list.push(sound_queue);
}

void FX_ConcreteImpact::OnUpdate()
{
	FX_BaseEffectActor::OnUpdate();
}
