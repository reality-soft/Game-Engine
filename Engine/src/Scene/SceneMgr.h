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
        ActorClass* GetActor(entt::entity actor_id);
        template <typename ActorClass, typename... Args>
        entt::entity AddActor(Args&&...args);
        template<typename ActorClass>
        ActorClass* GetPlayer(int player_num);
        template <typename ActorClass, typename... Args>
        entt::entity AddPlayer(Args&&...args);
        template <typename SceneClass>
        void SetScene();
        bool DestroyActor(entt::entity actor_id);
    public:
        int  GetNumOfActor();
    private:
        shared_ptr<Scene> cur_scene_;
    private:
        vector<entt::entity> players_;
        unordered_map<entt::entity, shared_ptr<Actor>> actors_;
    };

    template<typename ActorClass>
    inline ActorClass* SceneMgr::GetActor(entt::entity actor_id)
    {
        if (actors_.find(actor_id) == actors_.end())
            return nullptr;

        return dynamic_cast<ActorClass*>(weak_ptr(actors_.at(actor_id)).lock().get());
    }

    template<typename ActorClass, typename... Args>
    inline entt::entity SceneMgr::AddActor(Args&&...args)
    {
        shared_ptr<Actor> cur_actor = dynamic_pointer_cast<Actor>(make_shared<ActorClass>(args...));
        if (cur_actor == nullptr) {
            return entt::null;
        }

        cur_actor->OnInit(cur_scene_->GetRegistryRef());
        entt::entity cur_entity_id = cur_actor->GetEntityId();
        actors_.insert({ cur_entity_id, move(cur_actor) });

            return cur_entity_id;
        }

    template<typename ActorClass>
    inline ActorClass* SceneMgr::GetPlayer(int player_num)
    {
        if (player_num >= players_.size()) {
            return nullptr;
        }

        return dynamic_cast<ActorClass*>(weak_ptr(actors_.at(players_[player_num])).lock().get());
    }
    template<typename ActorClass, typename... Args>
    inline entt::entity SceneMgr::AddPlayer(Args&&...args)
    {
        shared_ptr<Actor> player = dynamic_pointer_cast<Actor>(make_shared<ActorClass>(args...));
        if (player == nullptr) {
            return entt::null;
        }

        player->OnInit(cur_scene_->GetRegistryRef());
        players_.push_back(player->GetEntityId());
        entt::entity cur_entity_id = player->GetEntityId();
        actors_.insert({ cur_entity_id, move(player) });
        return cur_entity_id;
    }

    template<typename SceneClass>
    inline void SceneMgr::SetScene()
    {
        cur_scene_ = make_shared<SceneClass>();
        OnInit();
    }
}
