#pragma once
#include "DataTypes.h"
#include "DllMacro.h"

using namespace reactphysics3d;

namespace KGCA41B
{
    class MouseRayCallback : public RaycastCallback
    {
    public:

        virtual decimal notifyRaycastHit(const RaycastInfo& info)
        {
            // Return a fraction of 1.0 to gather all hits 
            return decimal(1.0);
        }
    };

    class DLL_API PhysicsMgr
    {
        SINGLETON(PhysicsMgr);
#define PHYSICS KGCA41B::PhysicsMgr::GetInst()
    public:
        bool Init();
        void Update();
        void Release();

    public:
        Vector3 RaycastMouse(MouseRay* mouse_ray);


    private:
        PhysicsCommon physics_common_;
        PhysicsWorld* physics_world_;
        float accmulator = 0.0f;
    };
}

