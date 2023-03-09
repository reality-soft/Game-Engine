#include "TestGame.h"
#include "ComponentSystem.h"

void TestGame::OnInit()
{
	reality::RESOURCE->Init("../../Contents/");
	reality::FMOD_MGR->Init();
	sys_sound.OnCreate(reg_scene_); 
  
	reality::ComponentSystem::GetInst()->OnInit(reg_scene_);

	ent_player = reg_scene_.create();


	sys_render.OnCreate(reg_scene_);
	sys_camera.OnCreate(reg_scene_);
	sys_camera.TargetTag(reg_scene_, "Debug");

	level.CreateLevel(3, 100, 100, {8, 8});
	QUADTREE->Init(&level);
}

void TestGame::OnUpdate()
{
	reality::Material mat1;
}

void TestGame::OnRender()
{
	sys_render.OnUpdate(reg_scene_);
}

void TestGame::OnRelease()
{
	reality::RESOURCE->Release();
}

