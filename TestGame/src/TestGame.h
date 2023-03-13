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
	SkySphere sky_sphere;
	LightMeshLevel level;

	entt::registry reg_scene;

	reality::LightingSystem sys_light;
	reality::RenderSystem sys_render;
	reality::CameraSystem sys_camera;
};

