#pragma once
#include "Scene.h"

namespace reality {

    enum E_SceneType
    {
        SCENE_NONE = 0,
        POP = 1,
        START = 2,
        INGAME = 3,
        ENDING = 4,
    };

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
        template <typename SceneClass>
        void AddScene(E_SceneType type);
        shared_ptr<Scene> GetScene(E_SceneType type);
        void ChangeScene(E_SceneType scene_type);
        bool DestroyActor(entt::entity actor_id);
    private:
        void InternalChangeScene();
    public:
        int  GetNumOfActor();
        int  GetNumOfActor(string tag);
    private:
        E_SceneType         scene_to_change_;
        shared_ptr<Scene>   cur_scene_;
        unordered_map<E_SceneType, shared_ptr<Scene>> scenes;
    };

    template<typename ActorClass>
    inline ActorClass* SceneMgr::GetActor(entt::entity actor_id)
    {        
        return cur_scene_->GetActor<ActorClass>(actor_id);
    }

    template<typename ActorClass, typename... Args>
    inline entt::entity SceneMgr::AddActor(Args&&...args)
    {
        return cur_scene_->AddActor<ActorClass>(args...);
    }

    template<typename ActorClass>
    inline ActorClass* SceneMgr::GetPlayer(int player_num)
    {
        return cur_scene_->GetPlayer<ActorClass>(player_num);
    }

    template<typename ActorClass, typename... Args>
    inline entt::entity SceneMgr::AddPlayer(Args&&...args)
    {
        return cur_scene_->AddPlayer<ActorClass>(args...);
    }

    template<typename SceneClass>
    inline void SceneMgr::SetScene()
    {
        cur_scene_ = make_shared<SceneClass>();
        OnInit();
    }

    template<typename SceneClass>
    inline void SceneMgr::AddScene(E_SceneType type)
    {
        auto new_scene = make_shared<SceneClass>();
        scenes.insert({ type, new_scene });
        new_scene->OnInit();
    }
}
