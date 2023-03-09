#include "stdafx.h"
#include "Actor.h"

void reality::Actor::OnInit(entt::registry& registry)
{
	entity_id_ = registry.create();
}

void reality::Actor::OnUpdate(entt::registry& registry)
{

}