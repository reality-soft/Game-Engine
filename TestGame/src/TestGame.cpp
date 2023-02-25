#include "TestGame.h"
#include "ComponentSystem.h"

void TestGame::OnInit()
{
	KGCA41B::RESOURCE->Init("../../Contents/");

	level.ImportFromFile("../../Contents/BinaryPackage/Mountains.lv");

	level.vs_id_ = "LevelVS.cso";
	level.ps_id_ = "LevelPS.cso";
	level.texture_id = { "Ground.png" };


	KGCA41B::QUADTREE->Init(&level, 4);
	KGCA41B::FMOD_MGR->Init();

	for (int i = 0;i < 1000;i++) {
		KGCA41B::StaticObject actor;
		KGCA41B::C_Transform transform;
		KGCA41B::AABBShape collision_box;

		actor.OnInit(reg_scene, transform, collision_box, "AAA");
		actor_list.push_back(actor);
	}

	sys_sound.OnCreate(reg_scene); 
  
	KGCA41B::ComponentSystem::GetInst()->OnInit(reg_scene);

	ent_player = reg_scene.create();


	sys_render.OnCreate(reg_scene);
	ent_sound = reg_scene.create();

	for (int i = 0; i < 100; ++i)
	{
		XMVECTOR random_loacation = { rand() % 100 - 50, rand() % 100 - 50, rand() % 100 - 50, 0 };
		KGCA41B::AABBShape random_aabb = KGCA41B::AABBShape(random_loacation, 10.0f);
		int node = KGCA41B::QUADTREE->UpdateNodeObjectBelongs(0, random_aabb, reg_scene.create());;
		int a = 0;
	}

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

	sys_sound.OnUpdate(reg_scene);
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

