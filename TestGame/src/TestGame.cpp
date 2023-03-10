#include "TestGame.h"
#include "ComponentSystem.h"

void TestGame::OnInit()
{
	reality::RESOURCE->Init("../../Contents/");
	reality::FMOD_MGR->Init();
	sys_sound.OnCreate(reg_scene); 
  
	reality::ComponentSystem::GetInst()->OnInit(reg_scene);

	ent_player = reg_scene.create();


	sys_render.OnCreate(reg_scene);
	sys_camera.OnCreate(reg_scene);
	sys_camera.TargetTag(reg_scene, "Debug");
	level.ImportFromFile("../../Contents/BinaryPackage/Levels/jason.lv");
	QUADTREE->Init(&level);
}

void TestGame::OnUpdate()
{
	sys_camera.OnUpdate(reg_scene);
	QUADTREE->Frame(&sys_camera);
	reality::Material mat1;
}

void TestGame::OnRender()
{
	QUADTREE->Render();
	sys_render.OnUpdate(reg_scene);
}

void TestGame::OnRelease()
{
	reality::RESOURCE->Release();
}

