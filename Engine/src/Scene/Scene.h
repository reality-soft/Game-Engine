#pragma once
#include "TimeMgr.h"
#include "InputMgr.h"
#include "Actor.h"

namespace reality {
	class DLL_API Scene
	{
	public:
		Scene() = default;
		~Scene() = default;

		virtual void OnInit() = 0;
		virtual void OnBaseUpdate();
		virtual void OnUpdate() = 0;

		virtual void OnRender() = 0;
		virtual void OnRelease() = 0;
	public:
		entt::registry& GetRegistryRef() {
			return reg_scene_;
		}
	protected:
		entt::registry reg_scene_;
	private:
		vector<entt::entity> players_;
		unordered_map<entt::entity, shared_ptr<Actor>> actors_;
	public:
		vector<entt::entity>& GetPlayers();
		unordered_map<entt::entity, shared_ptr<Actor>>& GetActors();
	public:
		template<typename ActorClass>
		ActorClass* GetActor(entt::entity actor_id);
		template <typename ActorClass, typename... Args>
		entt::entity AddActor(Args&&...args);
		template<typename ActorClass>
		ActorClass* GetPlayer(int player_num);
		template <typename ActorClass, typename... Args>
		entt::entity AddPlayer(Args&&...args);
	};

    template<typename ActorClass>
    inline ActorClass* Scene::GetActor(entt::entity actor_id)
    {
        if (actors_.find(actor_id) == actors_.end())
            return nullptr;

        return dynamic_cast<ActorClass*>(weak_ptr(actors_.at(actor_id)).lock().get());
    }

    template<typename ActorClass, typename... Args>
    inline entt::entity Scene::AddActor(Args&&...args)
    {
        shared_ptr<Actor> cur_actor = dynamic_pointer_cast<Actor>(make_shared<ActorClass>(args...));
        if (cur_actor == nullptr) {
            return entt::null;
        }

        cur_actor->OnInit(reg_scene_);
        entt::entity cur_entity_id = cur_actor->GetEntityId();
        actors_.insert({ cur_entity_id, move(cur_actor) });

        return cur_entity_id;
    }

    template<typename ActorClass>
    inline ActorClass* Scene::GetPlayer(int player_num)
    {
        if (player_num >= players_.size()) {
            return nullptr;
        }

        return dynamic_cast<ActorClass*>(weak_ptr(actors_.at(players_[player_num])).lock().get());
    }
    template<typename ActorClass, typename... Args>
    inline entt::entity Scene::AddPlayer(Args&&...args)
    {
        shared_ptr<Actor> player = dynamic_pointer_cast<Actor>(make_shared<ActorClass>(args...));
        if (player == nullptr) {
            return entt::null;
        }

        player->OnInit(reg_scene_);
        players_.push_back(player->GetEntityId());
        entt::entity cur_entity_id = player->GetEntityId();
        actors_.insert({ cur_entity_id, move(player) });
        return cur_entity_id;
    }
}
