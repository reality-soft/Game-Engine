#pragma once
#include "Engine_Include.h"
#include "TestWidget.h"
#include "UI_Ingame_Actor.h"

using namespace reality;

class TestGame : public reality::Scene
{
public:
	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnRelease();

private:
  
	SkySphere sky_sphere;
	LightMeshLevel level;

	reality::LightingSystem sys_light;
	reality::RenderSystem sys_render;
	reality::CameraSystem sys_camera;
	reality::SoundSystem sys_sound;
	reality::EffectSystem sys_effect;
	reality::MovementSystem  sys_movement;
	reality::UISystem		sys_ui;

	StaticObject test_object;

private:
	TestWidget	test_window_;
	PropertyWidget gw_property_;
	UI_Ingame_Actor ingame_ui;
	void CreateBloodEffectFromRay();
	void CreateDustEffectFromRay();
};

