#include "stdafx.h"
#include "SceneMgr.h"

namespace reality{
	weak_ptr<Scene> SceneMgr::GetCurScene()
	{
		return weak_ptr<Scene>(cur_scene_);
	}

	weak_ptr<Actor> SceneMgr::GetActor(entt::entity actor_id)
	{
		return weak_ptr(actors_.at(actor_id));
	}

	weak_ptr<Actor> SceneMgr::GetPlayer(int player_num)
	{
		if (player_num >= players_.size()) {
			return weak_ptr<Actor>();
		}

		return weak_ptr(actors_.at(players_[player_num]));
	}

	void SceneMgr::OnInit()
	{
		if (cur_scene_.get() != nullptr) {
			cur_scene_->OnInit();
		}
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
