#pragma once
#include "Actor.h"
#include "UI_Image.h"
#include "UI_Text.h"
#include "UI_Button.h"
#include "UI_Slider.h"
namespace reality
{
	class DLL_API UIActor : public Actor
	{
	public:
		virtual void OnInit(entt::registry& registry);
		virtual void OnUpdate();
	};

}

