#include "stdafx.h"
#include "PointLightActor.h"
#include "ResourceMgr.h"

using namespace reality;

void PointLightActor::OnInit(entt::registry& registry)
{
	Actor::OnInit(registry);
	
	C_Transform transform;
	transform.world = XMMatrixIdentity();
	transform.local = XMMatrixIdentity();
	registry.emplace<C_Transform>(entity_id_, transform);
	
	transform_tree_.root_node = make_shared<TransformTreeNode>(TYPE_ID(reality::C_Transform));
	
	C_PointLight init_point_light_comp;
	auto& point_light_comp = registry.emplace<C_PointLight>(entity_id_, init_point_light_comp);
	point_light_comp.local = XMMatrixIdentity();
	transform_tree_.AddNodeToNode(TYPE_ID(reality::C_Transform), TYPE_ID(reality::C_PointLight));
	
	transform_tree_.root_node->OnUpdate(registry, entity_id_);
}

void PointLightActor::OnUpdate()
{
	auto& transform_comp = reg_scene_->get<C_Transform>(GetEntityId());
	auto& point_light_comp = reg_scene_->get<C_PointLight>(GetEntityId());
	XMVECTOR S, R, T;
	XMMatrixDecompose(&S, &R, &T, transform_comp.world);
	XMStoreFloat3(&point_light_comp.position, T);
}


void PointLightActor::Spawn(XMVECTOR pos, XMVECTOR rotation_q, XMVECTOR scale)
{
	auto& transform_comp = reg_scene_->get<C_Transform>(GetEntityId());
	XMMATRIX S, R, T;
	S = XMMatrixScalingFromVector(scale);
	R = XMMatrixRotationQuaternion(rotation_q);
	T = XMMatrixTranslationFromVector(pos);
	transform_comp.world = S * R * T;
	
	transform_tree_.root_node->OnUpdate(*reg_scene_, entity_id_, transform_comp.world);
	
	auto& point_light_comp = reg_scene_->get<C_PointLight>(GetEntityId());
	// lifetime = -1.0 -> Infinite Lifetime
	point_light_comp.lifetime = -1.0f;
	point_light_comp.timer = 0.0f;
}

void PointLightActor::Spawn(XMVECTOR pos, float lifetime, XMVECTOR rotation_q,  XMVECTOR scale)
{
	Spawn(pos, rotation_q, scale);
	
	auto& point_light_comp = reg_scene_->get<C_PointLight>(GetEntityId());
	point_light_comp.lifetime = lifetime;
}
