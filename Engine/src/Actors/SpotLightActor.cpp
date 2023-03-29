#include "stdafx.h"
#include "SpotLightActor.h"
#include "ResourceMgr.h"

using namespace reality;

void SpotLightActor::OnInit(entt::registry& registry)
{
	Actor::OnInit(registry);

	C_Transform transform;
	transform.world = XMMatrixIdentity();
	transform.local = XMMatrixIdentity();
	registry.emplace<C_Transform>(entity_id_, transform);

	transform_tree_.root_node = make_shared<TransformTreeNode>(TYPE_ID(reality::C_Transform));

	C_SpotLight init_spot_light_comp;
	auto& spot_light_comp = registry.emplace<C_SpotLight>(entity_id_, init_spot_light_comp);
	spot_light_comp.local = XMMatrixIdentity();
	transform_tree_.AddNodeToNode(TYPE_ID(reality::C_Transform), TYPE_ID(reality::C_SpotLight));

	transform_tree_.root_node->OnUpdate(registry, entity_id_);
}

void SpotLightActor::OnUpdate()
{
	auto& transform_comp = reg_scene_->get<C_Transform>(GetEntityId());
	auto& spot_light_comp = reg_scene_->get<C_SpotLight>(GetEntityId());
	XMVECTOR S, R, T;
	XMMatrixDecompose(&S, &R, &T, transform_comp.world);
	XMStoreFloat3(&spot_light_comp.position, T);
}

void SpotLightActor::Spawn(XMVECTOR pos, XMVECTOR rotation_q, XMVECTOR scale)
{
	auto& transform_comp = reg_scene_->get<C_Transform>(GetEntityId());
	XMMATRIX S, R, T;
	S = XMMatrixScalingFromVector(scale);
	R = XMMatrixRotationQuaternion(rotation_q);
	T = XMMatrixTranslationFromVector(pos);
	transform_comp.world = S * R * T;

	transform_tree_.root_node->OnUpdate(*reg_scene_, entity_id_, transform_comp.world);

	auto& spot_light_comp = reg_scene_->get<C_SpotLight>(GetEntityId());
	// lifetime = -1.0 -> Infinite Lifetime
	spot_light_comp.lifetime = -1.0f;
	spot_light_comp.timer = 0.0f;
}

void SpotLightActor::Spawn(XMVECTOR pos, float lifetime, XMVECTOR rotation_q,  XMVECTOR scale)
{
	Spawn(pos, rotation_q, scale);

	auto& spot_light_comp = reg_scene_->get<C_SpotLight>(GetEntityId());
	spot_light_comp.lifetime = lifetime;
}
