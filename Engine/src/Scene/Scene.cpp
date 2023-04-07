#include "stdafx.h"
#include "Scene.h"
#include "Actor.h"
#include "SceneMgr.h"

void reality::Scene::OnBaseUpdate()
{
    OnUpdate();

    for (const auto& actor : actors_) {
        actor.second->OnUpdate();
    }
}

vector<entt::entity>& reality::Scene::GetPlayers()
{
    return players_;
}

unordered_map<entt::entity, shared_ptr<reality::Actor>>& reality::Scene::GetActors()
{
    return actors_;
}
