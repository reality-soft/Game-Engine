#include "TestGame.h"
#include "SampleWidget.h"


using namespace KGCA41B;

void TestGame::OnInit()
{
	DINPUT->Init(ENGINE->GetWindowHandle(), ENGINE->GetInstanceHandle());

	//LoadResource();

	//ent_player = reg_scene.create();
	//ent_object = reg_scene.create();

	//CreatePlayer();
	//CreateCharacter();

	//sys_render.OnCreate(reg_scene);
	//sys_animation.OnCreate(reg_scene);
	//sys_camera.TargetTag(reg_scene, "Player");
	//sys_camera.OnCreate(reg_scene);
	//sys_input.OnCreate(reg_scene);

	//GUI
	SampleWidget* sample_widget = new SampleWidget;
	GUI->AddWidget("sample", sample_widget);
}

void TestGame::OnUpdate()
{
	//sys_input.OnUpdate(reg_scene);
	//sys_camera.OnUpdate(reg_scene);
}

void TestGame::OnRender()
{   
	//sys_animation.OnUpdate(reg_scene);
	//sys_render.OnUpdate(reg_scene);

	// GUI
	GUI->RenderWidgets();
}

void TestGame::OnRelease()
{
	RESOURCE->Release();
}

void TestGame::LoadResource()
{
	RESOURCE->Init("../../Contents/");
	RESOURCE->PushResource<FbxLoader>("player", "FBX/RumbaDancing.fbx");
	RESOURCE->PushResource<VsSkinned>("player", "Shader/SkinningVS.cso");
	RESOURCE->PushResource<PsDefault>("player", "Shader/SkinningPS.cso");
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

void TestGame::CreateCharacter()
{
	SkeletalMesh comp_skm;
	Skeleton comp_skeleton;
	Animation comp_animation;
	Material comp_material;
	Transform comp_transform;

	comp_skm.mesh_id = "player";
	comp_skm.shader_id = "player";

	comp_skeleton.skeleton_id = "player";
	comp_animation.anim_id = "player";

	comp_material.shader_id = "player";
	comp_material.texture_id = "null";

	reg_scene.emplace<SkeletalMesh>(ent_object, comp_skm);
	reg_scene.emplace<Skeleton>(ent_object, comp_skeleton);
	reg_scene.emplace<Animation>(ent_object, comp_animation);
	reg_scene.emplace<Material>(ent_object, comp_material);
	reg_scene.emplace<Transform>(ent_object, comp_transform);
}
