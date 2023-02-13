#include "TestGame.h"
#include "Engine/SpacePartition/Vector.h"
#include "ComponentSystem.h"

using namespace KGCA41B;

void TestGame::OnInit()
{
	for (int i = 0;i < 1000;i++) {
		Actor actor;
		actor.OnInit(reg_scene, KGCA41B::AABB<3>(Vector<3>::GetRandomVector(0, 300), Vector<3>::GetRandomVector(10, 20)));

		actor_list.push_back(actor);
	}
	

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

	for (Actor actor : actor_list) {
		actor.OnUpdate(reg_scene);
	}

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

