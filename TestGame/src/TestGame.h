#pragma once
#include "Engine_Include.h"

using namespace KGCA41B;

class TestGame : public KGCA41B::Scene
{
public:
	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnRelease();

private:
	entt::registry reg_scene;

	entt::entity ent_player;

	KGCA41B::RenderSystem sys_render;
	KGCA41B::CameraSystem sys_camera;
	KGCA41B::SoundSystem sys_sound;

	StaticObject test_object;
	SurfaceDefiner surface_definer;
};

