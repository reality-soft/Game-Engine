#pragma once
#include <reactphysics3d/reactphysics3d.h>
#include "common.h"
#include "DllMacro.h"

using namespace reactphysics3d;

namespace KGCA41B
{
    class DLL_API PhysicsMgr
    {
        SINGLETON(PhysicsMgr);
#define PHYSICS KGCA41B::PhysicsMgr::GetInst()
    public:
        bool Init();
        void Update();
        void Release();


    private:
        PhysicsCommon physics_common_;
        PhysicsWorld* physics_world_;
        float accmulator = 0.0f;
    };
}

