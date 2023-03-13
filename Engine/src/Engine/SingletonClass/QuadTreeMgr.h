#pragma once
#include "DllMacro.h"
#include "Collision.h"
#include "LightMeshLevel.h"
#include "../ECS/entt.hpp"
#include "../ECS/Systems/CameraSystem.h"

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
		vector<TriangleShape*> static_triangles;
	};

	class DLL_API QuadTreeMgr
	{
		SINGLETON(QuadTreeMgr)
#define QUADTREE QuadTreeMgr::GetInst()
	public:
		void Init(LightMeshLevel* level_to_devide, int max_depth = 5);
		void Frame(CameraSystem* applied_camera);
		void Release();

	public:
		void UpdatePhysics(float time_step);
		void ObjectCulling();

	private:
		UINT max_depth;
		UINT node_count = 0;

		vector<SpaceNode*> total_nodes_;
		vector<SpaceNode*> leaf_nodes_;
		SpaceNode* root_node_ = nullptr;



	private:
		SpaceNode* BuildTree(UINT depth, float row1, float col1, float row2, float col2);
		void SetStaticTriangles(SpaceNode* node);

	public:
		int									UpdateNodeObjectBelongs(int node_num, const AABBShape& object_area, entt::entity object_id);
		std::vector<int>					FindCollisionSearchNode(int node_num);
		std::unordered_set<entt::entity>	GetObjectListInNode(int node_num);

		LightMeshLevel* deviding_level_ = nullptr;
		Frustum camera_frustum_;

		bool wire_frame = false;
	};
}