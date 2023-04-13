#pragma once
#include "Collision.h"
#include "StaticMeshLevel.h"
#include "CameraSystem.h"
#include "StructuredBuffer.h"

#define MIN_HEIGHT -10000.f
#define MAX_HEIGHT  10000.f

namespace reality {

	class DLL_API SpaceNode 
	{
	public:
		SpaceNode(UINT num, UINT depth);
		~SpaceNode();

	public:
		void SetNode(float min_x, float min_z, float max_x, float max_z);
		void SetVisible(bool _visible);

	public:
		bool is_leaf = false;
		bool visible = false;
		UINT node_num;
		UINT node_depth;

	public:
		SpaceNode* child_node_[4] = { 0, };
		SpaceNode* parent_node = nullptr;
		UINT parent_node_index = 0;
		UINT child_node_index[4] = {0, 0, 0, 0};
			public:
		AABBShape area;
		BoundingBox culling_aabb;
		vector<TriangleShape> static_triangles;
		list<entt::entity> static_actors;
	};

	class DLL_API QuadTreeMgr
	{
		SINGLETON(QuadTreeMgr)
#define QUADTREE QuadTreeMgr::GetInst()

	private:
		entt::registry* registry_;

	public:
		void Init(StaticMeshLevel* level_to_devide, entt::registry& reg);

		void CreateQuadTreeData(int max_depth);
		void ImportQuadTreeData(string filename);
		bool CreatePhysicsCS();

		void Frame(CameraSystem* applied_camera);
		void UpdatePhysics(string cs_id);

		void Release();
	public:
		void SetSpaceHeight(float min_y, float max_y);
		void UpdateCapsules();
		RayCallback Raycast(const RayShape& ray);
		void RegistDynamicCapsule(entt::entity ent);
		bool RegistStaticSphere(entt::entity ent);

		SbTriangleCollision triangle_stbuffer;
		SbCapsuleCollision capsule_stbuffer;
		SbCollisionResult result_stbuffer;

		ComPtr<ID3D11Buffer> staging_buffer_;
		array<SbCollisionResult::Data, 64> collision_result_pool_;

		CbTransform capsule_mesh_transform;
		bool InitCollisionMeshes();
		void RenderCollisionMeshes();
		bool view_collisions_ = false;
		
	public:
		UINT visible_nodes = 0;

		// Physics Tree
	private:
		void RunPhysicsCS(string cs_id);
		void MovementByPhysicsCS();

		SpaceNode* ParentNodeQuery(C_CapsuleCollision* c_capsule, SpaceNode* node);
		bool	   LeafNodeQuery(C_CapsuleCollision* c_capsule, SpaceNode* node, vector<SpaceNode*>& out_nodes);
		bool	   IncludingNodeQuery(C_SphereCollision* c_sphere, SpaceNode* node, vector<SpaceNode*>& out_nodes);
		void	   NodeCulling(SpaceNode* node);

		UINT max_depth;
		UINT node_count = 0;

		SpaceNode* root_node_ = nullptr;
		map<UINT, SpaceNode*> total_nodes_;
		map<UINT, SpaceNode*> leaf_nodes_;
		map<entt::entity, C_CapsuleCollision*> dynamic_capsule_list;

	private:
		SpaceNode* BuildPhysicsTree(UINT depth, float min_x, float min_z, float max_x, float max_z);
		void SetStaticTriangles(SpaceNode* node);

	public:
		StaticMeshLevel* deviding_level_ = nullptr;
		BoundingFrustum camera_frustum_;
		bool wire_frame = false;
		void ExportQuadTreeData(string filename);

	private:
		void InitImported();
	};
}