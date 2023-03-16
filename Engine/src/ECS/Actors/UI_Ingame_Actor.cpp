#include "stdafx.h"
#include "UI_Ingame_Actor.h"
#include "Engine.h"

using namespace reality;

void UI_Ingame_Actor::OnInit(entt::registry& registry)
{
	UIActor::OnInit(registry);

	C_UI& ui_comp = reg_scene_->get<C_UI>(GetEntityId());

	// 무기 UI
	weapon_ui_ = make_shared<UI_Image>();
	weapon_ui_->InitImage("AR_01.png");
	weapon_ui_->SetLocalRectByCenter({ ENGINE->GetWindowSize().x / 2.0f, ENGINE->GetWindowSize().y * 5.0f / 6.0f }, 512.0f, 179.0f);
	shared_ptr<UI_Text> ammo_text = make_shared<UI_Text>();
	weapon_ui_->AddChildUI(ammo_text);
	ammo_text->InitText("30", { 265.0f, 55.0f }, 1.0f);
	ui_comp.ui_list.insert({ "Weapon UI", weapon_ui_ });

	// 미니맵 UI
	minimap_ui = make_shared<UI_Image>();
	minimap_ui->InitImage("Minimap_Border.png");
	minimap_ui->SetLocalRectByMin({ ENGINE->GetWindowSize().x * 4.0f / 5.0f, ENGINE->GetWindowSize().y * 1.0f / 10.0f }, 300.0f, 300.0f);
	ui_comp.ui_list.insert({ "Minimap UI", minimap_ui });

	// 플레이어 UI
	status_ui = make_shared<UI_Image>();
	status_ui->InitImage("BarHealth.png");
	status_ui->SetLocalRectByMin({ ENGINE->GetWindowSize().x * 1.0f / 12.0f, ENGINE->GetWindowSize().y * 9.0f / 10.0f }, 300.0f, 30.0f);
	ui_comp.ui_list.insert({ "Player UI", status_ui });

	// Objective UI
	objective_ui_ = make_shared<UI_Image>();
	objective_ui_->InitImage("Objective.png");
	objective_ui_->SetLocalRectByMin({ ENGINE->GetWindowSize().x * 4.0f / 5.0f, ENGINE->GetWindowSize().y * 4.0f / 10.0f }, 323.0f, 387.0f);
	ui_comp.ui_list.insert({ "Objective UI", objective_ui_ });
}

void UI_Ingame_Actor::OnUpdate()
{
}
