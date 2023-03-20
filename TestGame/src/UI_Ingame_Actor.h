#pragma once
#include "UIActor.h"
#include "UI_Image.h"
#include "UI_Text.h"
#include "UI_Button.h"
#include "UI_Minimap.h"

namespace reality
{
	class UI_Ingame_Actor : public UIActor
	{
	public:
		virtual void OnInit(entt::registry& registry);
		virtual void OnUpdate();
	private:
		shared_ptr<UI_Image>	weapon_ui_;
		shared_ptr<UI_Minimap>	minimap_ui;
		shared_ptr<UI_Image>	status_ui;
		shared_ptr<UI_Image>	objective_ui_;
		shared_ptr<UI_Image>	time_ui_;
	private:
		void CreateUI();
	};

}

