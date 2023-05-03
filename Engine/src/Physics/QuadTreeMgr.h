#pragma once
#include "Collision.h"
#include "StaticMeshLevel.h"
#include "CameraSystem.h"
#include "StructuredBuffer.h"
#include "Character.h"

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
		UINT child_node_index[4] = { 0, 0, 0, 0 };
	public:
		AABBShape area;
		BoundingBox culling_aabb;
		vector<TriangleShape> static_triangles;
		//list<entt::entity> static_actors;
	};

	class DLL_API QuadTreeMgr
	{
		SINGLETON(QuadTreeMgr)
#define QUADTREE QuadTreeMgr::GetInst()

	private:
		entt::registry* registry_;
		float accmulator = 1.0f / 60.0f;
		float time_bound = 0.0f;
	public:
		void Init(StaticMeshLevel* level_to_devide, entt::registry& reg);

		void CreateQuadTreeData(int max_depth);
		void ImportQuadTreeData(string mapdat_file);
		bool CreatePhysicsCS();

		void Frame(CameraSystem* applied_camera);
		void UpdatePhysics();

		void Release();
	public:
		void SetSpaceHeight(float min_y, float max_y);

		RayCallback Raycast(const RayShape& ray, entt::entity owner_ent);
		RayCallback RaycastActorOnly(const RayShape& ray, entt::entity owner_ent);
		RayCallback RaycastCarOnly(const RayShape& ray);
		RayCallback RaycastActorTargeted(const RayShape& ray, entt::entity target_ent);

		SbTriangleCollision triangle_stbuffer;
		SbCapsuleCollision capsule_stbuffer;
		SbSphereCollision sphere_stbuffer;
		SbCollisionResult result_stbuffer;

		ComPtr<ID3D11Buffer> staging_buffer_;
		array<SbCollisionResult::Data, 32> collision_result_pool_;
		array<SbCollisionResult::Data, 32> empty_pool_;

		CbTransform capsule_mesh_transform;
		bool InitCollisionMeshes();
		void RenderCollisionMeshes();
		bool view_collisions_ = false;

		const map<UINT, SpaceNode*>& GetLeafNodes();
	public:
		UINT visible_nodes = 0;
		UINT raycast_calculated = 0;
		XMMATRIX camera_rotation_;
		// Physics Tree
	private:
		void RunPhysicsCS(string cs_id);
		void MovementByPhysicsCS();
		void SphereUpdateByPhysicsCS();
		void CapsuleImpulse(entt::entity ent, C_CapsuleCollision& c_capsule);

		SpaceNode* ParentNodeQuery(C_CapsuleCollision& c_capsule, SpaceNode* node);
		SpaceNode* ParentNodeQuery(C_SphereCollision& c_sphere, SpaceNode* node);
		bool	   LeafNodeQuery(C_CapsuleCollision& c_capsule, SpaceNode* node, vector<SpaceNode*>& out_nodes);
		bool	   LeafNodeQuery(C_SphereCollision& c_sphere, SpaceNode* node, vector<SpaceNode*>& out_nodes);
		bool	   IncludingNodeQuery(C_SphereCollision& c_sphere, SpaceNode* node, vector<SpaceNode*>& out_nodes);
		void	   RaycastNodeQuery(const RayShape& ray, SpaceNode* node, map<float, RayCallback>& callbacks);
		void	   NodeCulling(SpaceNode* node);

		UINT max_depth;
		UINT node_count = 0;

		SpaceNode* root_node_ = nullptr;
		map<UINT, SpaceNode*> total_nodes_;
		map<UINT, SpaceNode*> leaf_nodes_;

	private:
		SpaceNode* BuildPhysicsTree(UINT depth, float min_x, float min_z, float max_x, float max_z);
		void SetStaticTriangles(SpaceNode* node);
		void AddStaticTriangles(const vector<TriangleShape>& triangles);

	public:
		StaticMeshLevel* deviding_level_ = nullptr;
		BoundingFrustum camera_frustum_;
		bool wire_frame = false;
		void ExportQuadTreeData(string filename);

	private:
		void InitImported();

		// GuideLine
	public:
		void ImportGuideLines(string mapdat_file, GuideType guide_type);
		vector<GuideLine>* GetGuideLines(string name);

		vector<TriangleShape>& GetCarTriangles();
		vector<TriangleShape> car_triagnles_;

		void SetBlockingFields(string name);
		vector<RayShape> blocking_fields_;
		XMVECTOR player_start_;

	private:
		map<string, vector<GuideLine>> guide_lines_;
		string mapdata_dir = "../../Contents/BinaryPackage/";
	};
}