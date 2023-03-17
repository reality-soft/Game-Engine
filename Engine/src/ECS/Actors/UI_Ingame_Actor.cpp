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
	weapon_ui_->InitImage("T_AR_01.png");
	weapon_ui_->SetLocalRectByMin({ 100.0f, ENGINE->GetWindowSize().y - 200.0f }, 512.0f, 179.0f);
	ui_comp.ui_list.insert({ "Weapon UI", weapon_ui_ });

	// 상태 + 아이템 UI
	status_ui = make_shared<UI_Image>();
	status_ui->InitImage("T_ItemUI.png");
	status_ui->SetLocalRectByCenter({ ENGINE->GetWindowSize().x / 2.0f, ENGINE->GetWindowSize().y - 150.0f }, 512.0f, 200.0f);

	auto hp_img = make_shared<UI_Image>();
	status_ui->AddChildUI(hp_img);
	hp_img->InitImage("T_HpBar.png");
	hp_img->SetLocalRectByCenter({ status_ui->rect_transform_.world_rect.width / 2.0f , 155.5f }, 400.0f, 30.0f);
	ui_comp.ui_list.insert({ "Status UI", status_ui });

	// 미니맵 UI
	minimap_ui = make_shared<UI_Image>();
	minimap_ui->InitImage("T_Minimap_Border.png");
	minimap_ui->SetLocalRectByMin({ ENGINE->GetWindowSize().x * 4.0f / 5.0f, ENGINE->GetWindowSize().y * 1.0f / 10.0f }, 300.0f, 300.0f);
	ui_comp.ui_list.insert({ "Minimap UI", minimap_ui });

	// 시간 UI
	time_ui_ = make_shared<UI_Image>();
	time_ui_->InitImage("T_TimeUI.png");
	time_ui_->SetLocalRectByMin({ 0, 0 }, 300.0f, 100.0f);
	ui_comp.ui_list.insert({ "time UI", time_ui_ });


	// Objective UI
	objective_ui_ = make_shared<UI_Image>();
	objective_ui_->InitImage("T_ObjectiveUI.png");
	objective_ui_->SetLocalRectByMin({ ENGINE->GetWindowSize().x * 4.0f / 5.0f, ENGINE->GetWindowSize().y * 4.0f / 10.0f }, 323.0f, 387.0f);
	ui_comp.ui_list.insert({ "Objective UI", objective_ui_ });
}

void UI_Ingame_Actor::OnUpdate()
{
}
