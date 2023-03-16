#include "TestGame.h"
#include "Player.h"

void TestGame::OnInit()
{
	GUI->AddWidget("property", &gw_property_);

	reality::RESOURCE->Init("../../Contents/");

	WRITER->Init();
	reality::ComponentSystem::GetInst()->OnInit(reg_scene_);

	sys_render.OnCreate(reg_scene_);
	sys_camera.OnCreate(reg_scene_);

// �׽�Ʈ UI
	test_ui_.OnInit(reg_scene_);
	CreateTestUI();
	sys_ui.OnCreate(reg_scene_);
  
	sys_camera.SetSpeed(1000);
	sys_light.OnCreate(reg_scene_);

	auto player_entity = SCENE_MGR->AddPlayer<Player>();
	sys_camera.TargetTag(reg_scene_, "Player");

	auto character_actor = SCENE_MGR->GetPlayer<Player>(0);
	// Key Settings
	INPUT_EVENT->Subscribe({ DIK_D }, std::bind(&Player::MoveRight, character_actor), KEY_HOLD);
	INPUT_EVENT->Subscribe({ DIK_W, DIK_D }, std::bind(&Player::MoveRightForward, character_actor), KEY_HOLD);
	INPUT_EVENT->Subscribe({ DIK_S, DIK_D }, std::bind(&Player::MoveRightBack, character_actor), KEY_HOLD);
	INPUT_EVENT->Subscribe({ DIK_A }, std::bind(&Player::MoveLeft, character_actor), KEY_HOLD);
	INPUT_EVENT->Subscribe({ DIK_W, DIK_A }, std::bind(&Player::MoveLeftForward, character_actor), KEY_HOLD);
	INPUT_EVENT->Subscribe({ DIK_S, DIK_A }, std::bind(&Player::MoveLeftBack, character_actor), KEY_HOLD);
	INPUT_EVENT->Subscribe({ DIK_W }, std::bind(&Player::MoveForward, character_actor), KEY_HOLD);
	INPUT_EVENT->Subscribe({ DIK_S }, std::bind(&Player::MoveBack, character_actor), KEY_HOLD);

	std::function<void()> idle = std::bind(&Player::Idle, character_actor);
	INPUT_EVENT->Subscribe({ DIK_D }, idle, KEY_UP);
	INPUT_EVENT->Subscribe({ DIK_S }, idle, KEY_UP);
	INPUT_EVENT->Subscribe({ DIK_W }, idle, KEY_UP);
	INPUT_EVENT->Subscribe({ DIK_A }, idle, KEY_UP);
	INPUT_EVENT->Subscribe({ DIK_SPACE }, idle, KEY_UP);

	INPUT_EVENT->Subscribe({ DIK_SPACE }, std::bind(&Player::Fire, character_actor), KEY_HOLD);

	sky_sphere.CreateSphere();
	level.Create("DeadPoly_FullLevel.ltmesh", "LevelVS.cso", "LevelGS.cso");
	QUADTREE->Init(&level);
	QUADTREE->RegisterDynamicCapsule(player_entity);

	gw_property_.AddProperty<float>("FPS", &TIMER->fps);
	gw_property_.AddProperty<int>("raycasted nodes", &QUADTREE->ray_casted_nodes);
	gw_property_.AddProperty<set<UINT>>("including nodes", &QUADTREE->including_nodes_num);
	gw_property_.AddProperty<XMVECTOR>("player pos", &QUADTREE->player_capsule_pos);
	gw_property_.AddProperty<int>("calculating triagnles", &QUADTREE->calculating_triagnles);
}

void TestGame::OnUpdate()
{
	sys_light.UpdateSun(sky_sphere);
	sys_camera.OnUpdate(reg_scene_);
	sys_light.OnUpdate(reg_scene_);
	sys_movement.OnUpdate(reg_scene_);
	QUADTREE->Frame(&sys_camera);
}

void TestGame::OnRender()
{
	sky_sphere.FrameRender(sys_camera.GetCamera());
	level.Update();
	level.Render();
	sys_render.OnUpdate(reg_scene_);
	sys_ui.OnUpdate(reg_scene_);

	GUI->RenderWidgets();
}

void TestGame::OnRelease()
{
	QUADTREE->Release();
	PHYSICS->Release();
	reality::RESOURCE->Release();
}

void TestGame::CreateEffectFromRay(XMVECTOR hitpoint)
{
	C_Effect& effect = reg_scene_.get<C_Effect>(effect_.GetEntityId());
	
	//effect.world = DirectX::XMMatrixAffineTransformation(S, O, R, T);
	effect.world = XMMatrixTranslationFromVector(hitpoint);
}

void TestGame::CreateTestUI()
{
	C_UI& ui_comp = reg_scene_.get<C_UI>(test_ui_.GetEntityId());
	// 무기 UI
	shared_ptr<UI_Image> weapon_image = make_shared<UI_Image>();
	weapon_image->InitImage("AR_01.png");
	weapon_image->SetLocalRectByMin({ 0, ENGINE->GetWindowSize().y * 5.0f / 6.0f }, 512.0f, 179.0f);
	shared_ptr<UI_Text> weapon_text = make_shared<UI_Text>();
	weapon_image->AddChildUI(weapon_text);
	weapon_text->InitText("AR_01", { weapon_image->rect_transform_.world_rect.width / 2.0f, weapon_image->rect_transform_.world_rect.height / 3.0f }, 0.2f);
	shared_ptr<UI_Text> ammo_text = make_shared<UI_Text>();
	weapon_image->AddChildUI(ammo_text);
	ammo_text->InitText("30/30", { weapon_image->rect_transform_.world_rect.width / 2.0f, weapon_image->rect_transform_.world_rect.height / 2.0f }, 0.2f);
	ui_comp.ui_list.insert({ "Weapon UI", weapon_image });

	// 미니맵 UI
	shared_ptr<UI_Image> minimap_image = make_shared<UI_Image>();
	minimap_image->InitImage("PreviewMinimap.png");
	//minimap_image->InitImage("Minimap_Border.png");
	minimap_image->SetLocalRectByMin({ ENGINE->GetWindowSize().x * 5.0f / 6.0f, ENGINE->GetWindowSize().y * 1.0f / 10.0f }, 248.0f, 245.0f);
	ui_comp.ui_list.insert({ "Minimap UI", minimap_image });

	// 플레이어 UI
	shared_ptr<UI_Image> player_image = make_shared<UI_Image>();
	player_image->InitImage("PreviewChr.png");
	player_image->SetLocalRectByMin({ 0, ENGINE->GetWindowSize().y * 1.0f / 10.0f }, 432.0f, 153.0f);
	//shared_ptr<UI_Image> player_image = make_shared<UI_Image>();
	//player_image->InitImage("PlayerBorder.png");
	//player_image->SetLocalRectByMin({ 0, ENGINE->GetWindowSize().y * 1.0f / 10.0f }, 432.0f, 153.0f);
	//
	//shared_ptr<UI_Image> hp_image = make_shared<UI_Image>();
	//player_image->AddChildUI(hp_image);
	//hp_image->InitImage("BarHealth.png");
	//hp_image->SetLocalRectByMin({ 170.0f, 47.0f }, 200.0f, 20.0f );
	//
	//shared_ptr<UI_Image> bq_image = make_shared<UI_Image>();
	//player_image->AddChildUI(bq_image);
	//bq_image->InitImage("BqBar.png");
	//bq_image->SetLocalRectByMin({ 170.0f, 80.0f }, 200.0f, 20.0f);
	ui_comp.ui_list.insert({ "Player UI", player_image });

	// Objective UI
	shared_ptr<UI_Image> obj_image = make_shared<UI_Image>();
	obj_image->InitImage("PreviewObj.png");
	obj_image->SetLocalRectByMin({ ENGINE->GetWindowSize().x * 5.0f / 6.0f, ENGINE->GetWindowSize().y * 4.0f / 10.0f }, 350.0f, 413.0f);

	ui_comp.ui_list.insert({ "Objective UI", obj_image });
}

