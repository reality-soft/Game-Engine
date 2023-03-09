#include "stdafx.h"
#include "Actor.h"

void reality::Actor::OnInit(entt::registry& registry)
{
	entity_id_ = registry.create();
	reg_scene_ = &registry;
}

void reality::Actor::OnUpdate()
{

}