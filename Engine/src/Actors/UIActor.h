#pragma once
#include "Actor.h"
namespace reality
{
	class DLL_API UIActor : public Actor
	{
	public:
		virtual void OnInit(entt::registry& registry);
		virtual void OnUpdate();
	};

}

