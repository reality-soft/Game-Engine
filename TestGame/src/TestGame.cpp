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
	// �̹��� �����
	shared_ptr<UI_Image> image = make_shared<UI_Image>();
	image->InitImage("Ground.png");
	image->SetLocalRectByCenter({ ENGINE->GetWindowSize().x / 2.0f, ENGINE->GetWindowSize().y / 2.0f }, 1000.0f, 500.0f);
	// �̹��� �Ʒ��� ��ư �����
	shared_ptr<UI_Button> button = make_shared<UI_Button>();
	image->AddChildUI(button);
	button->InitButton("Button Normal.png", "Button Hover.png", "Button Normal.png");
	button->SetLocalRectByCenter({ image->rect_transform_.local_rect.width / 2.0f, image->rect_transform_.local_rect.height / 2.0f }, 200.0f, 100.0f);
	// ��ư �Ʒ��� �ؽ�Ʈ �����
	shared_ptr<UI_Text> text = make_shared<UI_Text>();
	button->AddChildUI(text);
	text->InitText("Button", { button->rect_transform_.world_rect.width / 4.0f, button->rect_transform_.world_rect.height / 4.0f }, 0.2f);

	ui_comp.ui_list.insert({ "Test Image", image });
}

