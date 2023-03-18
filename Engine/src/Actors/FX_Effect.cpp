#include "stdafx.h"
#include "FX_Effect.h"
#include "ResourceMgr.h"

using namespace reality;

void FX_Effect::OnInit(entt::registry& registry, string effect_id)
{
	FX_BaseEffectActor::OnInit(registry);

	C_Effect initial_effect_comp;
	auto& effect_comp = registry.emplace<C_Effect>(entity_id_, initial_effect_comp);
	effect_comp.local = XMMatrixIdentity();

	effect_comp.effect_id = effect_id;

	if (effect_id != "")
	{
		effect_comp.effect = *RESOURCE->UseResource<Effect>(effect_id);
	}
	

	transform_tree_.AddNodeToNode(TYPE_ID(reality::C_Transform), TYPE_ID(reality::C_Effect));
	transform_tree_.root_node->OnUpdate(registry, entity_id_);
}

void FX_Effect::OnUpdate()
{
	FX_BaseEffectActor::OnUpdate();
	
}

void FX_Effect::AddEffect(map<string, Emitter>& emitter_list)
{
	auto& effect_comp = reg_scene_->get<C_Effect>(entity_id_);
	effect_comp.effect.emitters = emitter_list;
}

void FX_Effect::ResetEmitter()
{
	auto& effect_comp = reg_scene_->get<C_Effect>(entity_id_);
	effect_comp.effect.emitters.clear();
}
