#include "PhysicsMgr.h"
#include "TimeMgr.h"

using namespace KGCA41B;

bool PhysicsMgr::Init()
{
	// create world settings (gravity, or more detailed)
	PhysicsWorld::WorldSettings settings;
	settings.defaultVelocitySolverNbIterations = 20;
	settings.isSleepingEnabled = true;
	settings.gravity = Vector3(0, -9.81, 0);

	// create physics world with settings (world is single instance)
	physics_world_ = physics_common_.createPhysicsWorld(settings);
	if (physics_world_ == nullptr)
		return false;

	// Change the number of iterations of the velocity solver 
	physics_world_->setNbIterationsVelocitySolver(10);

	// Change the number of iterations of the position solver 
	physics_world_->setNbIterationsPositionSolver(5);
	

	return true;
}

void PhysicsMgr::Update()
{
	const float timestep = 1.0f / 60.0f;
	accmulator += TM_DELTATIME;

	while (accmulator >= timestep)
	{
		physics_world_->update(timestep);
		accmulator -= timestep;
	}	
}

void PhysicsMgr::Release()
{
	physics_common_.destroyPhysicsWorld(physics_world_);
}
