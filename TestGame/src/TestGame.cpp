#include "TestGame.h"
#include "ComponentSystem.h"

void TestGame::OnInit()
{
	DINPUT->Init(ENGINE->GetWindowHandle(), ENGINE->GetInstanceHandle());
	KGCA41B::RESOURCE->Init("../../Contents/");

	sys_input.OnCreate(reg_scene);
  
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

	KGCA41B::C_InputMapping debug_input;
	debug_input.tag = "Player";
	reg_scene.emplace<KGCA41B::C_InputMapping>(ent_player, debug_input);

	sys_camera.OnCreate(reg_scene);
	sys_camera.TargetTag(reg_scene, "Player");
	sys_input.OnCreate(reg_scene);


	FbxMgr::GetInst()->ImportAndSaveFbx("../../Contents/STM/LeeEnfieldMKIII.fbx");

	test_object.transform = XMMatrixIdentity();
	C_Transform transform;
	transform.local = XMMatrixIdentity();
	transform.world = XMMatrixIdentity();

	test_object.OnInit(reg_scene, transform, AABBShape(XMVectorZero(), 10.0f), "LeeEnfieldMKIII.stmesh");

	GUI->AddWidget("surface", &surface_definer);

	KGCA41B::Material mat1, mat2, mat3, mat4, mat5, mat6, mat7;
	mat1.texture_id = {   
	"Bayonet_DF.png",
	};
	mat2.texture_id = {
	"Ammo_DF.png",
	};
	mat3.texture_id = {
	"Bolt_DF.png",
	};
	mat4.texture_id = {
	"Magazine_DF.png",
	};
	mat5.texture_id = {
	"Metal_DF.png",
	};
	mat6.texture_id = {
	"Wood_DF.png",
	};
	mat7.texture_id = {
	"Strap_DF.png",
	};

	surface_definer.test_surface.materials.push_back(mat1);
	surface_definer.test_surface.materials.push_back(mat2);
	surface_definer.test_surface.materials.push_back(mat3);
	surface_definer.test_surface.materials.push_back(mat4);
	surface_definer.test_surface.materials.push_back(mat5);
	surface_definer.test_surface.materials.push_back(mat6);
	surface_definer.test_surface.materials.push_back(mat7);

	surface_definer.test_surface.CreateSurface();
}

void TestGame::OnUpdate()
{
	sys_input.OnUpdate(reg_scene);
	sys_camera.OnUpdate(reg_scene);
	test_object.OnUpdate(reg_scene);
}

void TestGame::OnRender()
{
	surface_definer.test_surface.SetSurface();
	sys_render.OnUpdate(reg_scene);
}

void TestGame::OnRelease()
{
	KGCA41B::RESOURCE->Release();
}

