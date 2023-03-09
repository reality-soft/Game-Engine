#pragma once
#include "DataTypes.h"
#include "DllMacro.h"
#include "entt.hpp"

using namespace reactphysics3d;

namespace reality
{
    class WorldRayCallback : public RaycastCallback
    {
    public:

        virtual decimal notifyRaycastHit(const RaycastInfo& info)
        {
            hitpoint = info.worldPoint;
            body = info.body;
            return decimal(0.0);
        }

        Vector3 hitpoint;
        CollisionBody* body; 
    };

    class DLL_API PhysicsMgr
    {
        SINGLETON(PhysicsMgr);
#define PHYSICS reality::PhysicsMgr::GetInst()
    public:
        bool Init();
        void Update();
        void Release();

    public:
        EventListener event_listener;
        PhysicsCommon physics_common_;

        PhysicsWorld* GetPhysicsWorld() { return physics_world_; }
        WorldRayCallback WorldPicking(const MouseRay& mouse_ray);
        bool ObjectPicking(const MouseRay& mouse_ray, CollisionBody* target);
    
    public:
        void AddSceneDynamic(entt::entity ent);
        void DestroySceneDynamic(entt::entity ent);

        void AddLevelStatic(const InstanceData& inst_data);
        void DestroyLevelStatic(string instance_id);

    public:
        std::map<entt::entity, CollisionBody*> scene_dynamics_;
        std::map<string, CollisionBody*>       level_statics_;
        CollisionBody*                         level_terrain_ = nullptr;

    private:
        PhysicsWorld* physics_world_;
        long double accmulator = 0;
    };
}