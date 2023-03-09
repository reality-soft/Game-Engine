#include "stdafx.h"
#include "SceneMgr.h"

void reality::SceneMgr::OnInit()
{
	if (cur_scene.get() != nullptr) {
		cur_scene->OnInit();
	}
}

void reality::SceneMgr::OnUpdate()
{
	if (cur_scene.get() != nullptr) {
		cur_scene->OnUpdate();
	}
}

void reality::SceneMgr::OnRender()
{
	if (cur_scene.get() != nullptr) {
		cur_scene->OnRender();
	}
}

void reality::SceneMgr::OnRelease()
{
	if (cur_scene.get() != nullptr) {
		cur_scene->OnRelease();
	}
}
