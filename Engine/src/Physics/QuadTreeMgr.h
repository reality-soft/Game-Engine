#pragma once
#include "Collision.h"
#include "StaticMeshLevel.h"
#include "CameraSystem.h"
#include "StructuredBuffer.h"

#define MIN_HEIGHT -10000.f
#define MAX_HEIGHT  10000.f

namespace reality {

	class DLL_API PhysicsNode 
	{
	public:
		PhysicsNode(UINT num, UINT depth);
		~PhysicsNode();

	public:
		void SetNode(float min_x, float min_z, float max_x, float max_z);

	public:
		bool is_leaf = false;
		UINT node_num;
		UINT node_depth;
		AABBShape area;
		PhysicsNode* child_node_[4] = { 0, };
		PhysicsNode* parent_node = nullptr;
		UINT parent_node_index = 0;
		UINT child_node_index[4] = {0, 0, 0, 0};
		vector<TriangleShape> static_triangles;
	};

	class DLL_API MeshNode
	{
	public:
		MeshNode(UINT num, UINT depth);
		~MeshNode();

		void SetNode(float min_x, float min_z, float max_x, float max_z);

		bool is_leaf = false;
		UINT node_num;
		UINT node_depth;
		AABBShape area;
		BoundingBox culling_aabb;
		MeshNode* child_node_[4] = { 0, };
		MeshNode* parent_node = nullptr;
		UINT parent_node_index = 0;
		UINT child_node_index[4] = { 0, 0, 0, 0 };
		StaticMesh separated_level_mesh_;
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

		void Render();

		void Release();
	public:
		void UpdateCapsules();
		//pair<RayCallback, entt::entity> RaycastAdjustActor(RayShape& ray);
		void RegistDynamicCapsule(entt::entity ent);

		SbTriangleCollision triangle_stbuffer;
		SbCapsuleCollision capsule_stbuffer;
		SbCollisionResult result_stbuffer;

		ComPtr<ID3D11Buffer> staging_buffer_;
		array<SbCollisionResult::Data, 64> collision_result_pool_;

	public:
		UINT culling_nodes = 0;
		vector<RayShape> blocking_lines;

		// Physics Tree
	private:
		void RunPhysicsCS(string cs_id);
		void MovementByPhysicsCS();

		PhysicsNode* ParentNodeQuery(C_CapsuleCollision* c_capsule, PhysicsNode* node);
		bool	   LeafNodeQuery(C_CapsuleCollision* c_capsule, PhysicsNode* node, vector<PhysicsNode*>& node_list);
		void	   NodeCasting(const RayShape& ray, PhysicsNode* node);
		void NodeCulling(MeshNode* node);

		UINT max_depth;
		UINT node_count = 0;

		PhysicsNode* root_physics_node_ = nullptr;
		map<UINT, PhysicsNode*> total_physics_nodes_;
		map<UINT, PhysicsNode*> leaf_physics_nodes_;
		map<float, PhysicsNode*> casted_physics_nodes_;

		map<entt::entity, C_CapsuleCollision*> dynamic_capsule_list;

	private:
		PhysicsNode* BuildPhysicsTree(UINT depth, float min_x, float min_z, float max_x, float max_z);
		void SetStaticTriangles(PhysicsNode* node);

		// MeshTree
	private:
		MeshNode* root_mesh_node_ = nullptr;
		map<UINT, MeshNode*> total_mesh_nodes_;
		map<UINT, MeshNode*> leaf_mesh_nodes_;
		MeshNode* BuildMeshTree(UINT depth, float min_x, float min_z, float max_x, float max_z);
		void SetMeshes(MeshNode* node);

	public:
		StaticMeshLevel* deviding_level_ = nullptr;
		BoundingFrustum camera_frustum_;
		bool wire_frame = false;
		void ExportQuadTreeData(string filename);

	private:
		void InitImported();
		void RenderNode(MeshNode* node);
	};
}