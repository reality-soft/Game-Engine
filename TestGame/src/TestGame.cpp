#include "TestGame.h"

using namespace KGCA41B;

void TestGame::OnInit()
{
	DINPUT->Init(ENGINE->GetWindowHandle(), ENGINE->GetInstanceHandle());

	FMOD_MGR->Init();
	RESOURCE->Init("D:/Contents");

	sys_sound.OnCreate(reg_scene); 
	sys_input.OnCreate(reg_scene);

	ent_player = reg_scene.create();
	ent_sound = reg_scene.create();

	CreatePlayer();
	CreateSound();
}

void TestGame::OnUpdate()
{
	FMOD_MGR->Update();
	int result = DINPUT->Update();

	static float time = 0;
	time += TIMER->GetDeltaTime();
	if (DINPUT->IsKeyPressed(DIK_SPACE) && time >= 2.0f)
	{
		CreateSound();
		time = 0;
	}
		

	sys_sound.OnUpdate(reg_scene);
	sys_input.OnUpdate(reg_scene);
}

void TestGame::OnRender()
{
	//sys_animation.OnUpdate(reg_scene);
	//sys_render.OnUpdate(reg_scene);
}

void TestGame::OnRelease()
{
	RESOURCE->Release();
	FMOD_MGR->Release();
}

void TestGame::CreatePlayer()
{
	auto& transform_comp = reg_scene.emplace<Transform>(ent_player);
	reg_scene.emplace<SoundListener>(ent_player);

	transform_comp.world_matrix.r[3].m128_f32[0] = 0;
	transform_comp.world_matrix.r[3].m128_f32[1] = 0;
	transform_comp.world_matrix.r[3].m128_f32[2] = 0;

	reg_scene.emplace<Transform>(ent_sound);
	reg_scene.emplace<SoundGenerator>(ent_sound);
}

void TestGame::CreateSound()
{
	auto& transform_comp = reg_scene.get<Transform>(ent_sound);
	auto& generator_comp = reg_scene.get<SoundGenerator>(ent_sound);

	static float dir = 100;
	dir *= -1.0f;

	transform_comp.world_matrix.r[3].m128_f32[0] = dir;
	transform_comp.world_matrix.r[3].m128_f32[1] = 0;
	transform_comp.world_matrix.r[3].m128_f32[2] = 0;

	SoundQueue que;
	que.sound_type = MUSIC;
	que.is_looping = true;
	que.sound_filename = "getitem.mp3";
	que.sound_volume = 100.0f;
	generator_comp.sound_queue_list.push(que);
}

