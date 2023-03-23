#include "TestGame.h"
#include "Player.h"
#include "Enemy.h"
#include "FX_BloodImpact.h"
#include "FX_ConcreteImpact.h"

void TestGame::OnInit()
{
	GUI->AddWidget("property", &gw_property_);

	reality::RESOURCE->Init("../../Contents/");
	//FbxImportOption option;
	//option.import_rotation = {90, 0, 180, 0};
	//option.import_scale = 10.0f;
	//reality::FBX->ImportAndSaveFbx("../../Contents/FBX/DeadPoly_Level_Collision.fbx", option);

	WRITER->Init();
	reality::ComponentSystem::GetInst()->OnInit(reg_scene_);

	sys_render.OnCreate(reg_scene_);
	sys_camera.OnCreate(reg_scene_);

	ingame_ui.OnInit(reg_scene_);
	sys_ui.OnCreate(reg_scene_);
  
	sys_camera.SetSpeed(1000);
	sys_light.OnCreate(reg_scene_);
	sys_effect.OnCreate(reg_scene_);

	auto player_entity = SCENE_MGR->AddPlayer<Player>();
	sys_camera.TargetTag(reg_scene_, "Player");

	auto character_actor = SCENE_MGR->GetPlayer<Player>(0);
	// Key Settings
	INPUT_EVENT->SubscribeKeyEvent({ DIK_D }, std::bind(&Player::MoveRight, character_actor), KEY_HOLD);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_W, DIK_D }, std::bind(&Player::MoveRightForward, character_actor), KEY_HOLD);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_S, DIK_D }, std::bind(&Player::MoveRightBack, character_actor), KEY_HOLD);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_A }, std::bind(&Player::MoveLeft, character_actor), KEY_HOLD);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_W, DIK_A }, std::bind(&Player::MoveLeftForward, character_actor), KEY_HOLD);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_S, DIK_A }, std::bind(&Player::MoveLeftBack, character_actor), KEY_HOLD);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_W }, std::bind(&Player::MoveForward, character_actor), KEY_HOLD);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_S }, std::bind(&Player::MoveBack, character_actor), KEY_HOLD);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_RETURN }, std::bind(&Player::ResetPos, character_actor), KEY_PUSH);

	INPUT_EVENT->SubscribeKeyEvent({ DIK_SPACE }, std::bind(&Player::Jump, character_actor), KEY_PUSH);

	std::function<void()> idle = std::bind(&Player::Idle, character_actor);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_D }, idle, KEY_UP);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_S }, idle, KEY_UP);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_W }, idle, KEY_UP);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_A }, idle, KEY_UP);

	INPUT_EVENT->SubscribeMouseEvent({ MouseButton::L_BUTTON }, std::bind(&Player::Fire, character_actor), KEY_HOLD);
	INPUT_EVENT->SubscribeMouseEvent({ MouseButton::L_BUTTON }, idle, KEY_UP);

	sky_sphere.CreateSphere();
	level.Create("DeadPoly_FullLevel.ltmesh", "LevelVS.cso", "LevelGS.cso", "DeadPoly_Level_Collision.ltmesh");
	level.ImportGuideLines("../../Contents/BinaryPackage/DeadPoly_Blocking1.mapdat", GuideLine::GuideType::eBlocking);
	level.ImportGuideLines("../../Contents/BinaryPackage/DeadPoly_NpcTrack.mapdat", GuideLine::GuideType::eNpcTrack);

	for (int i = 0;i < 10;i++) {
		auto enemy_entity = SCENE_MGR->AddActor<Enemy>();
		auto enemy_actor = SCENE_MGR->GetActor<Enemy>(enemy_entity);
	}

	QUADTREE->Init(&level, 3);

	gw_property_.AddProperty<float>("FPS", &TIMER->fps);
	gw_property_.AddProperty<int>("raycasted nodes", &QUADTREE->ray_casted_nodes);
	gw_property_.AddProperty<set<UINT>>("including nodes", &QUADTREE->including_nodes_num);
	gw_property_.AddProperty<XMVECTOR>("floor pos", &QUADTREE->player_capsule_pos);
	gw_property_.AddProperty<int>("calculating triagnles", &QUADTREE->calculating_triagnles);
}

void TestGame::OnUpdate()
{
	sys_light.UpdateSun(sky_sphere);
	sys_camera.OnUpdate(reg_scene_);
	sys_light.OnUpdate(reg_scene_);
	sys_movement.OnUpdate(reg_scene_);
	sys_effect.OnUpdate(reg_scene_);
	QUADTREE->Frame(&sys_camera);

	ingame_ui.OnUpdate();

	if (DINPUT->GetMouseState(L_BUTTON) == KeyState::KEY_PUSH)
		CreateBloodEffectFromRay();
	if (DINPUT->GetMouseState(R_BUTTON) == KeyState::KEY_PUSH)
		CreateDustEffectFromRay();
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
	reality::RESOURCE->Release();
}

void TestGame::CreateBloodEffectFromRay()
{
	RayCallback raycallback =  QUADTREE->RaycastAdjustLevel(sys_camera.CreateMouseRay(), 10000.0f);
	if(raycallback.success)
		EFFECT_MGR->SpawnEffectFromNormal<FX_BloodImpact>(raycallback.point, raycallback.normal, 1.0f);
}

void TestGame::CreateDustEffectFromRay()
{
	RayCallback raycallback = QUADTREE->RaycastAdjustLevel(sys_camera.CreateMouseRay(), 10000.0f);
	if (raycallback.success)
		EFFECT_MGR->SpawnEffectFromNormal<FX_ConcreteImpact>(raycallback.point, raycallback.normal, 1.0f);
}


