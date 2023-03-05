#include "TestGame.h"
#include "ComponentSystem.h"

void TestGame::OnInit()
{
	KGCA41B::RESOURCE->Init("../../Contents/");
	KGCA41B::FMOD_MGR->Init();
	sys_sound.OnCreate(reg_scene); 
  
	KGCA41B::ComponentSystem::GetInst()->OnInit(reg_scene);

	ent_player = reg_scene.create();


	sys_render.OnCreate(reg_scene);
	sys_camera.OnCreate(reg_scene);
	sys_camera.TargetTag(reg_scene, "Debug");

	Level level;
	level.CreateLevel(3, 8, 100, {8, 8});
	QUADTREE->Init(&level);
}

void TestGame::OnUpdate()
{
	KGCA41B::Material mat1;
}

void TestGame::OnRender()
{
	sys_render.OnUpdate(reg_scene);
}

void TestGame::OnRelease()
{
	KGCA41B::RESOURCE->Release();
}

