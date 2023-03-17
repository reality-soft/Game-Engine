#pragma once
#include "UIActor.h"
#include "UI_Image.h"
#include "UI_Text.h"
#include "UI_Button.h"

namespace reality
{
	class DLL_API UI_Ingame_Actor : public UIActor
	{
	public:
		virtual void OnInit(entt::registry& registry);
		virtual void OnUpdate();
	private:
		shared_ptr<UI_Image> weapon_ui_;
		shared_ptr<UI_Image> minimap_ui;
		shared_ptr<UI_Image> status_ui;
		shared_ptr<UI_Image> objective_ui_;
		shared_ptr<UI_Image> time_ui_;
	};

}

