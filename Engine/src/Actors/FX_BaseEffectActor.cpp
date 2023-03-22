#include "stdafx.h"
#include "FX_BaseEffectActor.h"
#include "ResourceMgr.h"

using namespace reality;

void FX_BaseEffectActor::OnInit(entt::registry& registry)
{
	Actor::OnInit(registry);

	C_Transform transform;
	transform.world = XMMatrixIdentity();
	transform.local = XMMatrixIdentity();
	registry.emplace<C_Transform>(entity_id_, transform);

	transform_tree_.root_node = make_shared<TransformTreeNode>(TYPE_ID(reality::C_Transform));

	C_Effect initial_effect_comp;
	auto& effect_comp = registry.emplace<C_Effect>(entity_id_, initial_effect_comp);
	effect_comp.local = XMMatrixIdentity();
	transform_tree_.AddNodeToNode(TYPE_ID(reality::C_Transform), TYPE_ID(reality::C_Effect));

	transform_tree_.root_node->OnUpdate(registry, entity_id_);
}

void FX_BaseEffectActor::OnUpdate()
{
	
}

void FX_BaseEffectActor::AddEffect(map<string, Emitter>& emitter_list)
{
	auto& effect_comp = reg_scene_->get<C_Effect>(entity_id_);
	effect_comp.effect.emitters = emitter_list;
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

	auto& effect_comp = reg_scene_->get<C_Effect>(entity_id_);
	// lifetime = -1.0 -> Infinite Lifetime
	effect_comp.effect_lifetime = -1.0f;
	effect_comp.effect_timer = 0.0f;
}

void FX_BaseEffectActor::Spawn(XMVECTOR pos, float lifetime, XMVECTOR rotation_q,  XMVECTOR scale)
{
	Spawn(pos, rotation_q, scale);

	auto& effect_comp = reg_scene_->get<C_Effect>(entity_id_);
	effect_comp.effect_lifetime = lifetime;
}

void FX_BaseEffectActor::ResetEmitter()
{
	auto& effect_comp = reg_scene_->get<C_Effect>(entity_id_);
	effect_comp.effect.emitters.clear();
}
