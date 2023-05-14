#include "stdafx.h"
#include "FX_BaseEffectActor.h"
#include "ResourceMgr.h"

using namespace reality;

void FX_BaseEffectActor::OnInit(entt::registry& registry)
{
	Actor::OnInit(registry);

	auto& transform = registry.emplace<C_Transform>(entity_id_);
	transform.world = XMMatrixIdentity();
	transform.local = XMMatrixIdentity();

	transform_tree_.root_node = make_shared<TransformTreeNode>(TYPE_ID(reality::C_Transform));
	transform_tree_.root_node->OnUpdate(registry, entity_id_);
}

void FX_BaseEffectActor::Spawn(XMVECTOR pos, XMVECTOR rotation_q, XMVECTOR scale)
{
	auto& transform_comp = reg_scene_->get<C_Transform>(GetEntityId());
	XMMATRIX S, R, T;
	S = XMMatrixScalingFromVector(scale);
	R = XMMatrixRotationQuaternion(rotation_q);
	T = XMMatrixTranslationFromVector(pos);
	transform_comp.world = S * R * T;

	transform_tree_.root_node->OnUpdate(*reg_scene_, entity_id_, transform_comp.world);
}

void FX_BaseEffectActor::AddEffectComponent(string effect_name, float lifetime)
{
	auto& effect_comp = reg_scene_->emplace<C_Effect>(entity_id_);
	effect_comp.local = XMMatrixIdentity();
	effect_comp.effect_id = effect_name;
	auto effect = RESOURCE->UseResource<Effect>(effect_name);
	if(effect)
		effect_comp.effect = *effect;
	// lifetime = -1.0 -> Infinite Lifetime
	effect_comp.effect_lifetime = lifetime;
	effect_comp.effect_timer = 0.0f;

	transform_tree_.AddNodeToNode(TYPE_ID(reality::C_Transform), TYPE_ID(reality::C_Effect));
	transform_tree_.root_node->OnUpdate(*reg_scene_, entity_id_);
}

void FX_BaseEffectActor::AddSoundGeneratorComponent()
{
	auto& sound_gen_comp = reg_scene_->emplace<C_SoundGenerator>(GetEntityId());
	sound_gen_comp.local = XMMatrixIdentity();

	transform_tree_.AddNodeToNode(TYPE_ID(reality::C_Transform), TYPE_ID(reality::C_SoundGenerator));
	transform_tree_.root_node->OnUpdate(*reg_scene_, entity_id_);
}

void FX_BaseEffectActor::AddPointLightComponent(string pointlight_name, float lifetime = -1.0f)
{
	auto& pointlight_comp = reg_scene_->emplace<C_PointLight>(entity_id_);
	pointlight_comp.point_light_id = pointlight_name;
	pointlight_comp.point_light = *(PointLight*)RESOURCE->UseResource<BaseLight>(pointlight_name);
	pointlight_comp.local = XMMatrixIdentity();
	pointlight_comp.lifetime = lifetime;
	pointlight_comp.timer = 0.0f;

	transform_tree_.AddNodeToNode(TYPE_ID(reality::C_Transform), TYPE_ID(reality::C_PointLight));
	transform_tree_.root_node->OnUpdate(*reg_scene_, entity_id_);
}

void FX_BaseEffectActor::AddSpotLightComponent(string spotlight_name, float lifetime)
{
	auto& spotlight_comp = reg_scene_->emplace<C_SpotLight>(entity_id_);
	spotlight_comp.spot_light_id = spotlight_name;
	spotlight_comp.spot_light = *(SpotLight*)RESOURCE->UseResource<BaseLight>(spotlight_name);
	spotlight_comp.local = XMMatrixIdentity();
	spotlight_comp.lifetime = lifetime;
	spotlight_comp.timer = 0.0f;

	transform_tree_.AddNodeToNode(TYPE_ID(reality::C_Transform), TYPE_ID(reality::C_SpotLight));
	transform_tree_.root_node->OnUpdate(*reg_scene_, entity_id_);
}

// Only Used for Tool
void FX_BaseEffectActor::AddEffect(map<string, Emitter>& emitter_list)
{
	auto& effect_comp = reg_scene_->get_or_emplace<C_Effect>(entity_id_);
	effect_comp.effect.emitters = emitter_list;
}

void FX_BaseEffectActor::ResetEmitter()
{
	auto& effect_comp = reg_scene_->get_or_emplace<C_Effect>(entity_id_);
	effect_comp.effect.emitters.clear();
}