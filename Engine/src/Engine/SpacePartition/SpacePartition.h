#pragma once
#include "../stdafx.h"
#include "../ECS/entt.hpp"

namespace KGCA41B {
#define OCT_TREE 3
#define QUAD_TREE 2
#define DIMENSION QUAD_TREE
#define MAX_DEPTH 4

	struct SpaceNode {
	public:
		int									depth;
		AABB<DIMENSION>						area;
		std::unordered_set<entt::entity>	object_list;
	};

	class SpacePartition
	{
		SINGLETON(SpacePartition)
	private:
		Vector<DIMENSION>		world_size_;
		constexpr static int	array_size_ = ((1 << ((MAX_DEPTH)*DIMENSION)) - 1) / ((1 << DIMENSION) - 1);
		SpaceNode				node_list_[array_size_];
	private:
		int		GetLeftMostChildOfNode(int depth, int node_num);
		int		GetLeftMostChildAtDepth(int depth);
		int		GetRightMostChildAtDepth(int depth);
		void	BuildTree(int depth, int node_num);
	public:
		int									FindNodeObjectBelongs(int node_num, const AABB<3>& object_area);
		std::vector<int>					FindCollisionSearchNode(int node_num, AABB<3>& object_area);
		std::unordered_set<entt::entity>	GetObjectListInNode(int node_num);
	public:
		void	Init(const Vector<DIMENSION> world_size);
		bool    Frame();
		bool    Render();
		bool    Release();
	private:
		const int delta[3][8] = { {0, 1, 0, 1, 0, 1, 0, 1 },
						{0, 0, 1, 1, 0, 0, 1, 1 },
						{0, 0, 0, 0, 1, 1, 1, 1 }
		};
	};
}