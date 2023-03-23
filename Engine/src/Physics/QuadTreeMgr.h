#pragma once
#include "Collision.h"
#include "LightMeshLevel.h"
#include "CameraSystem.h"

#define MIN_HEIGHT -10000.f
#define MAX_HEIGHT  10000.f
#define TIMESTEP_30FPS 1.0f / 30.0f
#define TIMESTEP_60FPS 1.0f / 60.0f

namespace reality {

	class DLL_API SpaceNode 
	{
	public:
		SpaceNode(UINT num, UINT depth);
		~SpaceNode();

	public:
		void SetNode(float min_x, float min_z, float max_x, float max_z);

	public:
		bool is_leaf = false;
		UINT node_num, node_depth;
		AABBShape area;
		SpaceNode* child_node_[4] = { 0, };
		std::unordered_set<entt::entity> object_list;
		vector<TriangleShape> static_triangles;

	};

	class DLL_API QuadTreeMgr
	{
		SINGLETON(QuadTreeMgr)
#define QUADTREE QuadTreeMgr::GetInst()
	public:
		void Init(LightMeshLevel* level_to_devide, int max_depth);
		void Frame(CameraSystem* applied_camera);
		void Release();

	public:
		RayCallback RaycastAdjustLevel(const RayShape& ray, float max_distance);
		RayCallback RaycastAdjustActor(const RayShape& ray);
		void RegistDynamicCapsule(entt::entity ent);

	public:
		int calculating_triagnles;
		int ray_casted_nodes;
		set<UINT> including_nodes_num;
		XMVECTOR player_capsule_pos;

		vector<RayShape> blocking_lines;

	private:
		void UpdatePhysics();
		void CheckBlockingLine();
		UINT max_depth;
		UINT node_count = 0;
		float physics_timestep = 1.0f / 60.0f;

		SpaceNode* root_node_ = nullptr;
		vector<SpaceNode*> total_nodes_;
		vector<SpaceNode*> leaf_nodes_;
		map<float, SpaceNode*> casted_nodes_;

		map<entt::entity, C_CapsuleCollision*> dynamic_capsule_list;

	private:
		void NodeCasting(const RayShape& ray, SpaceNode* node);
		void ObjectQueryByCapsule(CapsuleShape& capsule, SpaceNode* node, vector<SpaceNode*>& node_list);
		SpaceNode* BuildTree(UINT depth, float row1, float col1, float row2, float col2);
		void SetStaticTriangles(SpaceNode* node);

	public:
		LightMeshLevel* deviding_level_ = nullptr;
		Frustum camera_frustum_;

		bool wire_frame = false;
	};
}