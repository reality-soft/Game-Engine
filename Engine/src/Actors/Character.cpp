#include "stdafx.h"
#include "Character.h"
#include "TimeMgr.h"
#include "SceneMgr.h"

void reality::Character::OnInit(entt::registry& registry)
{
	Actor::OnInit(registry);
	registry.emplace_or_replace<C_Movement>(entity_id_, &transform_tree_);
	movement_component_ = registry.try_get<C_Movement>(entity_id_);
}

void reality::Character::OnUpdate()
{

}

void reality::Character::GravityFall(float _gravity)
{
	movement_component_->gravity += _gravity;
}