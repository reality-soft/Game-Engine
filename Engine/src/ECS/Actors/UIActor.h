#pragma once
#include "Actor.h"
namespace reality
{
	class DLL_API UIActor : public Actor
	{
		virtual void OnInit(entt::registry& registry);
		virtual void OnUpdate();
	};

}

