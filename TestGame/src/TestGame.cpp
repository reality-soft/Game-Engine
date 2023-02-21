#include "TestGame.h"
#include "ComponentSystem.h"

void TestGame::OnInit()
{
	//for (int i = 0;i < 1000;i++) {
	//	KGCA41B::Actor actor;
	//	actor.OnInit(reg_scene, KGCA41B::AABB<3>(KGCA41B::Vector<3>::GetRandomVector(0, 300), KGCA41B::Vector<3>::GetRandomVector(5, 10)));
	//	actor_list.push_back(actor);
	//}

	DINPUT->Init(ENGINE->GetWindowHandle(), ENGINE->GetInstanceHandle());
	KGCA41B::RESOURCE->Init("../../Contents/");

	level.ImportFromFile("../../Contents/BinaryPackage/level.lv");

	KGCA41B::QUADTREE->Init(&level, 4);
	KGCA41B::FMOD_MGR->Init();
	KGCA41B::RESOURCE->Init("../Contents");

	sys_sound.OnCreate(reg_scene); 
	sys_input.OnCreate(reg_scene);
  
	KGCA41B::ComponentSystem::GetInst()->OnInit(reg_scene);

	ent_player = reg_scene.create();
	ent_sound = reg_scene.create();


	for (int i = 0; i < 100; ++i)
	{
		XMVECTOR random_loacation = { rand() % 100 - 50, rand() % 100 - 50, rand() % 100 - 50, 0 };
		KGCA41B::AABBShape random_aabb = KGCA41B::AABBShape(random_loacation, 10.0f);
		int node = KGCA41B::QUADTREE->UpdateNodeObjectBelongs(0, random_aabb, reg_scene.create());;
		int a = 0;
	}

	//CreatePlayer();
	//CreateCharacter();

	//sys_render.OnCreate(reg_scene);
	//sys_animation.OnCreate(reg_scene);
	KGCA41B::Camera debug_camera;
	debug_camera.position = { 0, 100, -200, 0 };
	debug_camera.look = { 0, -1, 0, 0 };
	debug_camera.up = { 0, 1, 0, 0 };
	debug_camera.near_z = 1.f;
	debug_camera.far_z = 10000.f;
	debug_camera.fov = XMConvertToRadians(45);
	debug_camera.yaw = 0;
	debug_camera.pitch = 0;
	debug_camera.roll = 0;
	debug_camera.speed = 100;
	debug_camera.tag = "Player";
	reg_scene.emplace<KGCA41B::Camera>(ent_player, debug_camera);

	KGCA41B::InputMapping debug_input;
	debug_input.tag = "Player";
	reg_scene.emplace<KGCA41B::InputMapping>(ent_player, debug_input);

	sys_camera.OnCreate(reg_scene);
	sys_camera.TargetTag(reg_scene, "Player");
	sys_input.OnCreate(reg_scene);

}

void TestGame::OnUpdate()
{
	KGCA41B::FMOD_MGR->Update();

	for (KGCA41B::Actor actor : actor_list) {
		actor.OnUpdate(reg_scene);
	}

	sys_sound.OnUpdate(reg_scene);
	sys_input.OnUpdate(reg_scene);
	sys_camera.OnUpdate(reg_scene);

	KGCA41B::QUADTREE->Frame(&sys_camera);
}

void TestGame::OnRender()
{
	KGCA41B::QUADTREE->Render();
}

void TestGame::OnRelease()
{
	KGCA41B::RESOURCE->Release();
	KGCA41B::FMOD_MGR->Release();
}

