#pragma once
#include "DllMacro.h"
#include "Shape.h"
#include "Level.h"
#include "../ECS/entt.hpp"
#include "CameraSystem.h"

#define MIN_HEIGHT -1000.f
#define MAX_HEIGHT  1000.f

namespace KGCA41B {

	class LODCell
	{
	public:
		LODCell(UINT max_lod);
		bool Create(const UINT corners[4], UINT num_col);
		vector<vector<UINT>> lod_index_list;
		vector<ComPtr<ID3D11Buffer>> lod_index_buffer;
	};

	class DLL_API SpaceNode 
	{
	public:
		SpaceNode() = default;
		SpaceNode(UINT num, UINT depth);


	public:
		void SetNode(Level* level);
		void Render();
	public:
		UINT node_num, node_depth;
		AABBShape area;
		shared_ptr<SpaceNode> child_node_[4];
		std::unordered_set<entt::entity> object_list;
		UINT coner_index[4];
		LODCell* lod_cell = nullptr;
		UINT current_lod = 0;
	};

	class DLL_API QuadTreeMgr
	{
		SINGLETON(QuadTreeMgr)
#define QUADTREE QuadTreeMgr::GetInst()
	public:
		void Init(Level* level_to_devide);
		void Frame(CameraSystem* applied_camera);
		void Render();
		void Release();

	public:
		void UpdateLOD();
		void MapCulling(Frustum& frustum, SpaceNode* node);
		void ObjectCulling();
	private:
		XMINT2 world_size_;
		UINT max_depth;
		UINT node_count = 0;
		UINT max_lod;

		vector<shared_ptr<SpaceNode>> total_nodes_;
		vector<shared_ptr<SpaceNode>> leaf_nodes_;

		shared_ptr<SpaceNode> root_node_;


	private:
		SpaceNode* BuildTree(UINT depth, int row1, int row2, int col1, int col2);
		void RenderNode(SpaceNode* node_to_render);

	public:
		int									UpdateNodeObjectBelongs(int node_num, const AABBShape& object_area, entt::entity object_id);
		std::vector<int>					FindCollisionSearchNode(int node_num);
		std::unordered_set<entt::entity>	GetObjectListInNode(int node_num);

		shared_ptr<Level> deviding_level_;
		Frustum camera_frustum_;
	};
}