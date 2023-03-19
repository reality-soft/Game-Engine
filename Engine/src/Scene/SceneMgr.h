#pragma once
#include "Scene.h"
#include "Actor.h"

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
        entt::registry& GetRegistry();
        weak_ptr<Scene> GetCurScene();

    public:
        template<typename ActorClass>
        ActorClass* GetActor(entt::entity actor_id)
        {
            return dynamic_cast<ActorClass*>(weak_ptr(actors_.at(actor_id)).lock().get());
        }
        template <typename ActorClass>
        entt::entity AddActor() {
            shared_ptr<Actor> cur_actor = dynamic_pointer_cast<Actor>(make_shared<ActorClass>(ActorClass()));
            if (cur_actor == nullptr) {
                return entt::null;
            }

            cur_actor->OnInit(cur_scene_->GetRegistryRef());
            entt::entity cur_entity_id = cur_actor->GetEntityId();
            actors_.insert({ cur_entity_id, move(cur_actor)});

            return cur_entity_id;
        }

        template<typename ActorClass>
        ActorClass* GetPlayer(int player_num)
        {
            if (player_num >= players_.size()) {
                return nullptr;
            }

            return dynamic_cast<ActorClass*>(weak_ptr(actors_.at(players_[player_num])).lock().get());
        }
        template <typename ActorClass>
        entt::entity AddPlayer() {
            shared_ptr<Actor> player = dynamic_pointer_cast<Actor>(make_shared<ActorClass>(ActorClass()));
            if (player == nullptr) {
                return entt::null;
            }

            player->OnInit(cur_scene_->GetRegistryRef());
            players_.push_back(player->GetEntityId());
            entt::entity cur_entity_id = player->GetEntityId();
            actors_.insert({ cur_entity_id, move(player)});
            return cur_entity_id;
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
