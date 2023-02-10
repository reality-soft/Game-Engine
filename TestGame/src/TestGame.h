#pragma once
#include "Engine_include.h"

class TestGame : public KGCA41B::Scene
{
public:
	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnRelease();

private:
	KGCA41B::Actor actor;

	entt::registry reg_scene;

	entt::entity ent_player;
	entt::entity ent_sound;

	KGCA41B::RenderSystem sys_render;
	KGCA41B::CameraSystem sys_camera;
	KGCA41B::InputSystem sys_input;
	KGCA41B::AnimationSystem sys_animation;
	KGCA41B::SoundSystem sys_sound;
};

