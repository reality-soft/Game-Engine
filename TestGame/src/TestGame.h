#pragma once
#include "Engine_Include.h"

using namespace reality;

class TestGame : public reality::Scene
{
public:
	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnRelease();

private:
	entt::registry reg_scene;

	entt::entity ent_player;

	reality::RenderSystem sys_render;
	reality::CameraSystem sys_camera;
	reality::SoundSystem sys_sound;

	StaticObject test_object;
	Level level;
};

