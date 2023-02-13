#include "TestGame.h"
#include "Engine/SpacePartition/Vector.h"
#include "ComponentSystem.h"

void TestGame::OnInit()
{
	for (int i = 0;i < 1000;i++) {
		KGCA41B::Actor actor;
		actor.OnInit(reg_scene, KGCA41B::AABB<3>(KGCA41B::Vector<3>::GetRandomVector(0, 300), KGCA41B::Vector<3>::GetRandomVector(5, 10)));

		actor_list.push_back(actor);
	}
	

	DINPUT->Init(ENGINE->GetWindowHandle(), ENGINE->GetInstanceHandle());

	KGCA41B::FMOD_MGR->Init();
	KGCA41B::RESOURCE->Init("D:/Contents");

	sys_sound.OnCreate(reg_scene); 
	sys_input.OnCreate(reg_scene);
  
	KGCA41B::ComponentSystem::GetInst()->OnInit(reg_scene);

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
	KGCA41B::FMOD_MGR->Update();
	int result = DINPUT->Update();

	for (KGCA41B::Actor actor : actor_list) {
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
	KGCA41B::RESOURCE->Release();
	KGCA41B::FMOD_MGR->Release();
}

