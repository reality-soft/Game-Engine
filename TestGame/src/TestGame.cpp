#include "TestGame.h"
#include "ComponentSystem.h"

using namespace KGCA41B;

void TestGame::OnInit()
{
	DINPUT->Init(ENGINE->GetWindowHandle(), ENGINE->GetInstanceHandle());

	FMOD_MGR->Init();
	RESOURCE->Init("D:/Contents");

	sys_sound.OnCreate(reg_scene); 
	sys_input.OnCreate(reg_scene);
  
	ComponentSystem::GetInst()->OnInit(reg_scene);

	ent_player = reg_scene.create();
	ent_sound = reg_scene.create();


	//CreatePlayer();
	//CreateCharacter();

	//sys_render.OnCreate(reg_scene);
	//sys_animation.OnCreate(reg_scene);
	//sys_camera.TargetTag(reg_scene, "Player");
	//sys_camera.OnCreate(reg_scene);
	//sys_input.OnCreate(reg_scene);

}

void TestGame::OnUpdate()
{
	FMOD_MGR->Update();
	int result = DINPUT->Update();
		

	sys_sound.OnUpdate(reg_scene);
	sys_input.OnUpdate(reg_scene);
}

void TestGame::OnRender()
{
}

void TestGame::OnRelease()
{
	RESOURCE->Release();
	FMOD_MGR->Release();
}

