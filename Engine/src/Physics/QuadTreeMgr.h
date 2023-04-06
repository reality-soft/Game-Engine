#pragma once
#include "Collision.h"
#include "StaticMeshLevel.h"
#include "CameraSystem.h"
#include "StructuredBuffer.h"

#define MIN_HEIGHT -10000.f
#define MAX_HEIGHT  10000.f
#define TIMESTEP_30FPS 1.0f / 30.0f
#define TIMESTEP_60FPS 1.0f / 60.0f

namespace reality {

	struct CollisionResult
	{
		int result;
		XMFLOAT3 position;
	};

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
		SpaceNode* parent_node = nullptr;
		std::unordered_set<entt::entity> object_list;
		vector<TriangleShape> static_triangles;

	};

	class DLL_API QuadTreeMgr
	{
		SINGLETON(QuadTreeMgr)
#define QUADTREE QuadTreeMgr::GetInst()

	private:
		entt::registry* registry_;

	public:
		void Init(StaticMeshLevel* level_to_devide, int max_depth, entt::registry& reg);
		void Frame(CameraSystem* applied_camera);
		void Release();
	public:
		void UpdateCapsules();
		RayCallback RaycastAdjustLevel(const RayShape& ray, float max_distance);
		pair<RayCallback, entt::entity> RaycastAdjustActor(const RayShape& ray);
		void RegistDynamicCapsule(entt::entity ent);

		bool CreatePhysicsCS();
		void RunPhysicsCS(string cs_id);

		//ComPtr<ID3D11Buffer> triangle_data_buffer_;
		//ComPtr<ID3D11Buffer> capsule_data_buffer_;
		//ComPtr<ID3D11Buffer> collision_result_buffer_;

		//ComPtr<ID3D11ShaderResourceView> triangle_data_srv_;
		//ComPtr<ID3D11ShaderResourceView> capsule_data_srv_;
		//ComPtr<ID3D11UnorderedAccessView> collision_result_uav_;

		SbTriangleCollision triangle_stbuffer;
		SbCapsuleCollision capsule_stbuffer;
		SbCollisionResult result_stbuffer;

		//array<CapsuleShape, 64> capsule_data_pool_;

		ComPtr<ID3D11Buffer> staging_buffer_;
		array<CollisionResult, 64> collision_result_pool_;

	public:
		int calculating_triagnles;
		set<UINT> including_nodes_num;
		XMVECTOR player_capsule_pos;

		vector<RayShape> blocking_lines;

	private:
		void UpdatePhysics();
		void CheckTriangle(entt::entity ent, CapsuleShape& capsule, vector<SpaceNode*> nodes);
		void CheckBlockingLine(entt::entity ent, CapsuleShape& capsule);

		SpaceNode* ParentNodeQuery(C_CapsuleCollision* c_capsule, SpaceNode* node);
		bool	   LeafNodeQuery(C_CapsuleCollision* c_capsule, SpaceNode* node, vector<SpaceNode*>& node_list);
		void	   NodeCasting(const RayShape& ray, SpaceNode* node);

		UINT max_depth;
		UINT node_count = 0;
		float physics_timestep = 1.0f / 120.0f;

		SpaceNode* root_node_ = nullptr;
		map<UINT, SpaceNode*> total_nodes_;
		map<UINT, SpaceNode*> leaf_nodes_;
		map<float, SpaceNode*> casted_nodes_;

		map<entt::entity, C_CapsuleCollision*> dynamic_capsule_list;

	private:
		SpaceNode* BuildTree(UINT depth, float row1, float col1, float row2, float col2);
		void SetStaticTriangles(SpaceNode* node);

	public:
		StaticMeshLevel* deviding_level_ = nullptr;
		Frustum camera_frustum_;

		bool wire_frame = false;
	};
}