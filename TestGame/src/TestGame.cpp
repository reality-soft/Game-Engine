#include "TestGame.h"

using namespace KGCA41B;

void TestGame::OnInit()
{
	DINPUT->Init(ENGINE->GetWindowHandle(), ENGINE->GetInstanceHandle());

	ent_player = reg_scene.create();
	ent_object = reg_scene.create();

	CreatePlayer();
	CreateObjectFromFbx("Resource/RumbaDancing.fbx");

	sys_render.OnCreate(reg_scene);
	sys_animation.OnCreate(reg_scene);
	sys_camera.TargetTag(reg_scene, "Player");
	sys_camera.OnCreate(reg_scene);
	sys_input.OnCreate(reg_scene);
}

void TestGame::OnUpdate()
{
	sys_input.OnUpdate(reg_scene);
	sys_camera.OnUpdate(reg_scene);
}

void TestGame::OnRender()
{
	sys_animation.OnUpdate(reg_scene);
	sys_render.OnUpdate(reg_scene);
}

void TestGame::OnRelease()
{
}

void TestGame::CreatePlayer()
{
	Camera comp_camera;
	comp_camera.position = { 0, 0, -50, 0 };
	comp_camera.look = { -50, 0, 0, 0 };
	comp_camera.target = { -50, 0, 0, 0 };
	comp_camera.up = { 0, 1, 0, 0 };
	comp_camera.near_z = 1.f;
	comp_camera.far_z = 10000.f;
	comp_camera.fov = XMConvertToRadians(45);
	comp_camera.yaw = 0;
	comp_camera.pitch = 0;
	comp_camera.roll = 0;
	comp_camera.speed = 50;
	comp_camera.tag = "Player";



	InputMapping comp_inputmapping;
	comp_inputmapping.tag = "Player";

	reg_scene.emplace<Camera>(ent_player, comp_camera);
	reg_scene.emplace<InputMapping>(ent_player, comp_inputmapping);
}

void TestGame::CreateObjectFromFbx(string filepath)
{
	FbxLoader fbx_loader;
	fbx_loader.LoadFromFbxFile(filepath);
	fbx_loader.LoadAnimation(FbxTime::eFrames60);

	SkeletalMesh comp_skm;
	comp_skm.vs_skinned.LoadCompiled(L"Shader/SkinningVS.cso");
	Skeleton comp_skeleton;
	for (auto mesh : fbx_loader.out_mesh_list)
	{
		SingleMesh<SkinnedVertex> single_mesh;
		single_mesh.vertices = mesh->skinned_vertices;
		comp_skm.mesh_list.push_back(single_mesh);

		comp_skeleton.bind_poses.merge(mesh->bind_poses);
	}

	Animation comp_animation;
	for (auto anim : fbx_loader.out_anim_list)
	{
		comp_animation.anim_track = anim->animations;
		comp_animation.start_frame = anim->start_frame;
		comp_animation.end_frame = anim->end_frame;
	}

	Material comp_material;
	comp_material.ps_default.LoadCompiled(L"Shader/SkinningPS.cso");

	Transform comp_transform;

	reg_scene.emplace<SkeletalMesh>(ent_object, comp_skm);
	reg_scene.emplace<Skeleton>(ent_object, comp_skeleton);
	reg_scene.emplace<Animation>(ent_object, comp_animation);
	reg_scene.emplace<Material>(ent_object, comp_material);
	reg_scene.emplace<Transform>(ent_object, comp_transform);

	fbx_loader.Destroy();
}
