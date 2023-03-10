#pragma once
#include "Engine_Include.h"
#include "TestWidget.h"

using namespace reality;

class TestGame : public reality::Scene
{
public:
	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnRelease();

private:
	Level level;

	entt::entity ent_player;

	reality::RenderSystem sys_render;
	reality::CameraSystem sys_camera;
	reality::SoundSystem sys_sound;
	reality::EffectSystem sys_effect;

	StaticObject test_object;

private:
	TestWidget	test_window_;
	FX_Effect	effect_;
	reality::WorldRayCallback callback;
	void CreateEffectFromRay(XMVECTOR hitpoint);
};

