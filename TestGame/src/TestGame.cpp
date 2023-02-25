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

	KGCA41B::C_Camera debug_camera;
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
	reg_scene.emplace<KGCA41B::C_Camera>(ent_player, debug_camera);

	sys_camera.OnCreate(reg_scene);
	sys_camera.TargetTag(reg_scene, "Player");

}

void TestGame::OnUpdate()
{
	KGCA41B::FMOD_MGR->Update();

	FbxMgr::GetInst()->ImportAndSaveFbx("../../Contents/STM/LeeEnfieldMKIII.fbx");

	test_object.transform = XMMatrixIdentity();
	C_Transform transform;
	transform.local = XMMatrixIdentity();
	transform.world = XMMatrixIdentity();

	test_object.OnInit(reg_scene, transform, AABBShape(XMVectorZero(), 10.0f), "LeeEnfieldMKIII.stmesh");

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

