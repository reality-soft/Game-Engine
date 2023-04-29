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
	shared_ptr<Scene> SceneMgr::GetScene(E_SceneType type)
	{
		if (scenes.find(type) == scenes.end())
			return NULL;
		else
			return scenes[type];
	}
	void SceneMgr::ChangeScene(E_SceneType scene_type)
	{
		scene_to_change_ = scene_type;
	}
	bool SceneMgr::DestroyActor(entt::entity actor_id)
	{
		if (cur_scene_->GetActors().find(actor_id) == cur_scene_->GetActors().end())
			return false;
		GetRegistry().destroy(actor_id);
		cur_scene_->GetActors().erase(actor_id);
		return true;
	}
	void SceneMgr::InternalChangeScene()
	{
		if (scenes.find(scene_to_change_) != scenes.end())
		{
			if (cur_scene_ != nullptr)
			{
				cur_scene_->OnRelease();
			}
				
			cur_scene_ = scenes[scene_to_change_];
			COMPONENT->OnInit(cur_scene_->GetRegistryRef());
		}
		scene_to_change_ = E_SceneType::SCENE_NONE;
	}
	void SceneMgr::OnInit()
	{
		if (cur_scene_.get() != nullptr) {
			cur_scene_->OnInit();
			COMPONENT->OnInit(cur_scene_->GetRegistryRef());
		}
	}
	
	void SceneMgr::OnUpdate()
	{
		if (cur_scene_.get() != nullptr) {
			cur_scene_->OnBaseUpdate();
		}

		if (scene_to_change_ != E_SceneType::SCENE_NONE)
		{
			InternalChangeScene();
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

	int  SceneMgr::GetNumOfActor() {
		return cur_scene_->GetActors().size();
	}

	int SceneMgr::GetNumOfActor(string tag)
	{
		int num = 0;
		auto actors = cur_scene_->GetActors();
		for (const auto& actor : actors)
		{
			if (actor.second->tag == tag)
				num++;
		}


		return num;
	}
}
