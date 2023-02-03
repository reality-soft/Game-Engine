#include "TestGame.h"

using namespace KGCA41B;

void TestGame::OnInit()
{
	DINPUT->Init(ENGINE->GetWindowHandle(), ENGINE->GetInstanceHandle());
	LoadResource();

	sys_sound.OnCreate(reg_scene);

	ent_player = reg_scene.create();
	CreatePlayer();
	CreateCharacter();

	ent_sound = reg_scene.create();
	CreateSound();
}

void TestGame::OnUpdate()
{
	sys_sound.OnUpdate(reg_scene);
}

void TestGame::OnRender
{   
	sys_animation.OnUpdate(reg_scene);
	sys_render.OnUpdate(reg_scene);
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
  
	sys_sound.OnRelease();
}

void TestGame::CreatePlayer()
{
	auto& transform_comp = reg_scene.emplace<Transform>(ent_player);
	reg_scene.emplace<SoundListener>(ent_player);

	transform_comp.world_matrix.r[3].m128_f32[0] = 0;
	transform_comp.world_matrix.r[3].m128_f32[1] = 0;
	transform_comp.world_matrix.r[3].m128_f32[2] = 0;
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
void TestGame::CreateSound()
{
	auto& transform_comp = reg_scene.emplace<Transform>(ent_sound);
	auto& generator_comp = reg_scene.emplace<SoundGenerator>(ent_sound);

	transform_comp.world_matrix.r[3].m128_f32[0] = -200;
	transform_comp.world_matrix.r[3].m128_f32[1] = 0;
	transform_comp.world_matrix.r[3].m128_f32[2] = 0;

	SoundQueue que;
	que.sound_type = SFX;
	que.is_looping = true;
	que.sound_filename = L"D:/Sound/getitem.mp3";
	que.sound_volume = 100.0f;
	generator_comp.sound_queue_list.push(que);
}

