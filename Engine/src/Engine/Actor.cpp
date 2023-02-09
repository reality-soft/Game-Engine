#include "Actor.h"


KGCA41B::Actor::Actor(entt::registry& registry)
{
	OnInit(registry);
}

void KGCA41B::Actor::OnInit(entt::registry& registry)
{
	entity_id_ = registry.create();
	
	entt::meta_type meta;
	decltype(meta);
}