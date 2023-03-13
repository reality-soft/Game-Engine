#include "TestGame.h"

void TestGame::OnInit()
{
	reality::RESOURCE->Init("../../Contents/");

	reality::ComponentSystem::GetInst()->OnInit(reg_scene_);

	sys_render.OnCreate(reg_scene);
	sys_camera.OnCreate(reg_scene);
	sys_camera.TargetTag(reg_scene, "Debug");
	sys_camera.SetSpeed(1000);
	sys_light.OnCreate(reg_scene);

	sky_sphere.CreateSphere();
	level.Create("DeadPoly_FullLevel.ltmesh", "LevelVS.cso", "LevelGS.cso");
}

void TestGame::OnUpdate()
{
	sys_light.UpdateSun(sky_sphere);
	sys_camera.OnUpdate(reg_scene);
	sys_light.OnUpdate(reg_scene);

	sys_camera.OnUpdate(reg_scene);
}

void TestGame::OnRender()
{
	sky_sphere.FrameRender(sys_camera.GetCamera());
	level.Update();
	level.Render();
	sys_render.OnUpdate(reg_scene);
}

void TestGame::OnRelease()
{
	reality::RESOURCE->Release();
}

