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

Vector3 PhysicsMgr::RaycastMouse(MouseRay* mouse_ray)
{
	Ray new_ray(mouse_ray->start_point, mouse_ray->end_point);

	MouseRayCallback ray_callback;
	RaycastInfo raycast_info;

	physics_world_->raycast(new_ray, &ray_callback);

	decimal result = ray_callback.notifyRaycastHit(raycast_info);

	return raycast_info.worldPoint;
}
