#include "TestGame.h"

void TestGame::OnInit()
{
	GUI->AddWidget("test_window", &test_window_);

	reality::RESOURCE->Init("../../Contents/");
	reality::FMOD_MGR->Init();
	PHYSICS->Init();
	sys_sound.OnCreate(reg_scene_); 
  
	reality::ComponentSystem::GetInst()->OnInit(reg_scene_);

	ent_player = reg_scene_.create();

	sys_render.OnCreate(reg_scene_);
	sys_camera.OnCreate(reg_scene_);
	sys_effect.OnCreate(reg_scene_);
	sys_camera.TargetTag(reg_scene_, "Debug");
	level.ImportFromFile("../../Contents/BinaryPackage/Levels/jason.lv");
	//QUADTREE->Init(&level);

	effect_.OnInit(reg_scene_, "sample_effect");
}

void TestGame::OnUpdate()
{
	sys_camera.OnUpdate(reg_scene_);
	sys_effect.OnUpdate(reg_scene_);
	//QUADTREE->Frame(&sys_camera);
	level.Update();
	PHYSICS->Update();

	
	reality::MouseRay current_ray = sys_camera.CreateMouseRay();
	callback = PHYSICS->WorldPicking(current_ray);
	XMVECTOR current_point;
	XMVECTOR current_normal;
	RPtoXM(callback.hitpoint, current_point);
	RPtoXM(callback.hitnormal, current_normal);
	XMVECTOR S = { 1.0f, 1.0f, 1.0f, 0.0f };
	XMVECTOR O = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR R = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	//XMVECTOR R = XMQuaternionRotationRollPitchYawFromVector(current_normal);
	XMVECTOR T = current_point;

	test_window_.SetPickingVector(current_point);

	if (DINPUT->GetMouseState(L_BUTTON) == KEY_HOLD)
		CreateEffectFromRay(current_point);
	
}

void TestGame::OnRender()
{
	//QUADTREE->Render();
	level.Render(false);
	sys_render.OnUpdate(reg_scene_);
	GUI->RenderWidgets();
}

void TestGame::OnRelease()
{
	PHYSICS->Release();
	reality::RESOURCE->Release();
}

void TestGame::CreateEffectFromRay(XMVECTOR hitpoint)
{
	C_Effect& effect = reg_scene_.get<C_Effect>(effect_.GetEntityId());
	
	
	
	
	//effect.world = DirectX::XMMatrixAffineTransformation(S, O, R, T);
	effect.world = XMMatrixTranslationFromVector(hitpoint);
}

