#include "stdafx.h"
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
	physics_world_->setEventListener(&event_listener);

	return true;
}

void PhysicsMgr::Update()
{
	// Constant physics time step 
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

WorldRayCallback KGCA41B::PhysicsMgr::WorldPicking(const MouseRay& mouse_ray)
{
	WorldRayCallback raycast_callback;
	Ray ray(mouse_ray.start_point, mouse_ray.end_point);
	physics_world_->raycast(ray, &raycast_callback);

	return raycast_callback;
}

bool KGCA41B::PhysicsMgr::ObjectPicking(const MouseRay& mouse_ray, CollisionBody* target)
{
	Ray ray(mouse_ray.start_point, mouse_ray.end_point);
	RaycastInfo ray_info;
	target->raycast(ray, ray_info);
	if (ray_info.body == target)
		return true;

	return false;
}
