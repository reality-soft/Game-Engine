#include "TestGame.h"

using namespace KGCA41B;

void TestGame::OnInit()
{
	DINPUT->Init(ENGINE->GetWindowHandle(), ENGINE->GetInstanceHandle());

	sys_sound.OnCreate(reg_scene);

	ent_player = reg_scene.create();
	CreatePlayer();

	ent_sound = reg_scene.create();
	CreateSound();
}

void TestGame::OnUpdate()
{
	sys_sound.OnUpdate(reg_scene);
}

void TestGame::OnRender()
{
	//sys_animation.OnUpdate(reg_scene);
	//sys_render.OnUpdate(reg_scene);
}

void TestGame::OnRelease()
{
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

