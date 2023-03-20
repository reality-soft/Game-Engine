#include "stdafx.h"
#include "SceneMgr.h"

namespace reality{
	entt::registry& SceneMgr::GetRegistry()
	{
		return cur_scene_.get()->GetRegistryRef();
	}
	weak_ptr<Scene> SceneMgr::GetCurScene()
	{
		return weak_ptr<Scene>(cur_scene_);
	}
	void SceneMgr::OnInit()
	{
		if (cur_scene_.get() != nullptr) {
			cur_scene_->OnInit();
		}
		COMPONENT->OnInit(cur_scene_->GetRegistryRef());
	}

	void SceneMgr::OnUpdate()
	{
		if (cur_scene_.get() != nullptr) {
			cur_scene_->OnUpdate();
		}

		for (const auto& actor : actors_) {
			actor.second->OnUpdate();
		}
	}

	void SceneMgr::OnRender()
	{
		if (cur_scene_.get() != nullptr) {
			cur_scene_->OnRender();
		}
	}

	void SceneMgr::OnRelease()
	{
		if (cur_scene_.get() != nullptr) {
			cur_scene_->OnRelease();
		}
	}
}
