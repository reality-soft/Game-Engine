#pragma once
#include "DllMacro.h"
#include "Shape.h"
#include "Level.h"
#include "../ECS/entt.hpp"

#define MAX_HEIGHT  100000
#define MIN_HEIGHT -100000

namespace KGCA41B {

	class DLL_API SpaceNode 
	{
	public:
		SpaceNode() = default;
		SpaceNode(UINT num, UINT depth);


	public:
		void SetNode(Level* level);

	public:
		UINT node_num, node_depth;
		AABBShape area;

		shared_ptr<SpaceNode> child_node_[4];
		std::unordered_set<entt::entity> object_list;

		UINT coner_index[4];
		vector<UINT> index_list;
		ComPtr<ID3D11Buffer> index_buffer;
	};

	class DLL_API QuadTreeMgr
	{
		SINGLETON(QuadTreeMgr)
#define QUADTREE QuadTreeMgr::GetInst()
	public:
		void Init(Level* level_to_devide, Camera* camera_to_apply, int _max_depth);
		bool Frame();
		bool Render();
		bool Release();

	public:
		void UpdateNodes();
		void MapCulling();
		void ObjectCulling();

	private:
		XMINT2 world_size_;
		UINT max_depth;
		UINT node_count = 0;
		//constexpr static int	array_size_ = ((1 << ((MAX_DEPTH)*DIMENSION)) - 1) / ((1 << DIMENSION) - 1);
		//SpaceNode				node_list_[array_size_];
		vector<shared_ptr<SpaceNode>> total_nodes_;
		shared_ptr<SpaceNode> root_node_;


	private:
		//int		GetLeftMostChildOfNode(int depth, int node_num);
		//int		GetLeftMostChildAtDepth(int depth);
		//int		GetRightMostChildAtDepth(int depth);
		SpaceNode* BuildTree(UINT depth, int row1, int row2, int col1, int col2);

		//public:
		//	int									UpdateNodeObjectBelongs(int node_num, const AABB<3>& object_area, entt::entity object_id);
		//	std::vector<int>					FindCollisionSearchNode(int node_num, AABB<3>& object_area);
		//	std::unordered_set<entt::entity>	GetObjectListInNode(int node_num);

		shared_ptr<Level> deviding_level_;
		shared_ptr<Camera> apllied_camera_;
	};
}