#pragma once
#include "../../ECS/Actors/Actor.h"
#include "Scene.h"

namespace reality {
    class DLL_API SceneMgr {
        SINGLETON(SceneMgr)
#define SCENE_MGR SceneMgr::GetInst()
    public:
        void OnInit();
        void OnUpdate();
        void OnRender();
        void OnRelease();
    public:
        weak_ptr<Scene> GetCurScene();
        weak_ptr<Actor> GetActor(entt::entity actor_id);
        weak_ptr<Actor> GetPlayer(int player_num);
    public:
        template <typename ActorClass>
        bool AddActor() {
            shared_ptr<Actor> cur_actor = dynamic_pointer_cast<Actor>(make_shared<ActorClass>());
            if (cur_actor == nullptr) {
                return false;
            }

            cur_actor->OnInit(cur_scene_->GetRegistryRef());
            actor_.insert({ cur_actor->GetEntityId(), move(cur_actor)});
            return true;
        }
        template <typename ActorClass>
        bool AddPlayer() {
            shared_ptr<Actor> player = dynamic_pointer_cast<Actor>(make_shared<ActorClass>());
            if (player == nullptr) {
                return false;
            }

            player->OnInit(cur_scene_->GetRegistryRef());
            players_.push_back(player->GetEntityId());
            actors_.insert({ player->GetEntityId(), move(player)});
            return true;
        }
        template <typename SceneClass>
        void SetScene() {
            cur_scene_ = make_shared<SceneClass>();
            cur_scene_->OnInit();
        }

    private:
        shared_ptr<Scene> cur_scene_;
    private:
        vector<entt::entity> players_;
        unordered_map<entt::entity, shared_ptr<Actor>> actors_;
    };
}
