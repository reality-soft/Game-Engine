#include "stdafx.h"
#include "UIActor.h"

void reality::UIActor::OnInit(entt::registry& registry)
{
	Actor::OnInit(registry);

	C_UI ui_comp;
	registry.emplace<C_UI>(entity_id_, ui_comp);
}

void reality::UIActor::OnUpdate()
{
}
