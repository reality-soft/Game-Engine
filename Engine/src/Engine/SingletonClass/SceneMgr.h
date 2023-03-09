#pragma once

#include "stdafx.h"
#include "Scene.h"

namespace reality {
    class DLL_API SceneMgr {
        SINGLETON(SceneMgr)
#define SCENE_MGR SceneMgr::GetInst()
    public:
        template <typename SceneClass>
        void SetScene() {
            cur_scene = make_shared<SceneClass>();
            cur_scene->OnInit();
        }

        weak_ptr<Scene> GetCurScene() {
            return weak_ptr<Scene>(cur_scene);
        }

        virtual void OnInit();
        virtual void OnUpdate();
        virtual void OnRender();
        virtual void OnRelease();
    private:
        shared_ptr<Scene> cur_scene;
    };
}
