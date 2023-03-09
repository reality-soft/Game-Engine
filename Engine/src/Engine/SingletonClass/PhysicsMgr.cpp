#include "stdafx.h"
#include "PhysicsMgr.h"
#include "TimeMgr.h"

using namespace reality;

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
	float timestep = 1.0f / 60.0f;

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

WorldRayCallback reality::PhysicsMgr::WorldPicking(const MouseRay& mouse_ray)
{
	WorldRayCallback raycast_callback;
	Ray ray(mouse_ray.start_point, mouse_ray.end_point);
	physics_world_->raycast(ray, &raycast_callback);

	return raycast_callback;
}

bool reality::PhysicsMgr::ObjectPicking(const MouseRay& mouse_ray, CollisionBody* target)
{
	Ray ray(mouse_ray.start_point, mouse_ray.end_point);
	RaycastInfo ray_info;
	target->raycast(ray, ray_info);
	if (ray_info.body == target)
		return true;

	return false;
}

void reality::PhysicsMgr::AddSceneDynamic(entt::entity ent)
{
}

void reality::PhysicsMgr::DestroySceneDynamic(entt::entity ent)
{
}

void reality::PhysicsMgr::AddLevelStatic(const InstanceData& inst_data)
{
	string instance_id = inst_data.instance_id;
	CollisionBody* collision_body = nullptr;


	//if (level_statics_.find(instance_id) != level_statics_.end())
	//{
	//	physics_common_.createBoxShape();
	//	collision_body =  physics_world_->createCollisionBody(inst_data.collision_transform);
	//	collision_body->addCollider();
	//	level_statics_.insert(instance_id);
	//}
}

void reality::PhysicsMgr::DestroyLevelStatic(string instance_id)
{
}
