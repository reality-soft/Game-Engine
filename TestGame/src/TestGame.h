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
	Level	level;
	UIActor test_ui_;

	entt::entity ent_player;

	RenderSystem	sys_render;
	CameraSystem	sys_camera;
	SoundSystem		sys_sound;
	EffectSystem	sys_effect;
	UISystem		sys_ui;

	StaticObject test_object;

private:
	TestWidget	test_window_;
	FX_Effect	effect_;
	reality::WorldRayCallback callback;
	void CreateEffectFromRay(XMVECTOR hitpoint);
	void CreateTestUI();
};

