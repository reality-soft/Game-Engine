#pragma once
#include "../ECS/entt.hpp"

namespace KGCA41B {
#define Dimension 2
#define MaxDepth 4

	struct SpaceNode {
	public:
		int									depth;
		AABB<Dimension>						area;
		std::unordered_set<entt::entity>	object_list;
	};

	class SpacePartition
	{
		SINGLETON(SpacePartition)
	private:
		Vector<Dimension>		world_size_;
		constexpr static int	array_size_ = ((1 << ((MaxDepth)*Dimension)) - 1) / ((1 << Dimension) - 1);
		SpaceNode				node_list_[array_size_];
	private:
		int	GetLeftMostChildOfNode(int depth, int node_num) 
		{
			if (node_num == 0) return 1;
			int left_most_node = GetLeftMostChildAtDepth(depth);
			int next_layer_left_most_node = GetLeftMostChildAtDepth(depth + 1);
			int children_per_node = 1 << Dimension;
			return next_layer_left_most_node + (node_num - left_most_node) * children_per_node;
		}
		int	GetLeftMostChildAtDepth(int depth) 
		{
			int children_per_node = 1 << Dimension;
			return ((1 << ((depth - 1) * Dimension)) - 1) / (children_per_node - 1);
		}
		int	GetRightMostChildAtDepth(int depth) 
		{
			int children_per_node = 1 << Dimension;
			return ((1 << (depth * Dimension)) - 1) / (children_per_node - 1) - 1;
		}
	private:
		void BuildTree(int depth, int node_num) 
		{
			if (depth >= MaxDepth) return;
			int left_most_child = GetLeftMostChildOfNode(depth, node_num);
			Vector<Dimension> node_position;
			Vector<Dimension> node_size = node_list_[node_num].area.Size() / 2;
			int children_per_node = 1 << Dimension;
			for (int cur_child_node = left_most_child; cur_child_node < left_most_child + children_per_node; cur_child_node++) {
				for (int j = 0; j < Dimension; j++) {
					node_position[j] = node_list_[node_num].area.MinCoord()[j] + delta[j][cur_child_node - left_most_child] * node_size[j];
				}
				node_list_[cur_child_node].area.Set(node_position, node_size);
				node_list_[cur_child_node].depth = depth;
				BuildTree(depth + 1, cur_child_node);
			}
		}
	public:
		int	FindNodeObjectBelongs(int node_num, const AABB<3>& object_area) 
		{
			std::queue<int> bfs_queue;
			int cur_node = node_num;
			int children_per_node = 1 << Dimension;

			AABB<Dimension> object_area_d;
#if (Dimension == 3)
			object_area_d = object_area;
#elif (Dimension == 2)
			Vector<3> center_coord = object_area.CenterCoord();
			Vector<3> size = object_area.Size();
			object_area_d.Set({ center_coord[0], center_coord[1] }, { size[0], size[1] });
#endif
			do {
				int left_most_child = GetLeftMostChildOfNode(node_list_[cur_node].depth, cur_node);
				for (int cur_child_node = left_most_child; cur_child_node < left_most_child + children_per_node; cur_child_node++) {
					if (left_most_child >= array_size_) continue;

					AABB<Dimension> cur_node_area = node_list_[cur_child_node].area;
					
					if (Collision<Dimension>::CubeToCube(object_area_d, cur_node_area) == CollisionType::C_R_IN_L) {
						bfs_queue.push(cur_child_node);
						break;
					}
				}
				if (bfs_queue.empty()) break;
				cur_node = bfs_queue.front();
				bfs_queue.pop();
			} while (cur_node);
			return cur_node;
		}
		std::vector<int> FindCollisionSearchNode(int node_num, AABB<3>& object_area) 
		{
			std::queue<int> bfs_queue;
			std::vector<int> node_to_search;

			AABB<Dimension> object_area_d;
#if (Dimension == 3)
			object_area_d = object_area;
#elif (Dimension == 2)
			Vector<3> center_coord = object_area.CenterCoord();
			Vector<3> size = object_area.Size();
			object_area_d.Set({ center_coord[0], center_coord[1] }, { size[0], size[1] });
#endif

			int cur_node = node_num;
			int children_per_node = 1 << Dimension;
			do {
				int left_most_child = GetLeftMostChildOfNode(node_list_[cur_node].depth, cur_node);
				for (int cur_child = left_most_child; cur_child < left_most_child + children_per_node; cur_child++) {
					if (left_most_child >= array_size_) continue;

					AABB<Dimension> cur_child_node_area = node_list_[cur_child].area;

					if (Collision<Dimension>::CubeToCube(object_area_d, cur_child_node_area) != CollisionType::C_OUT) {
						bfs_queue.push(cur_child);
						node_to_search.push_back(cur_child);
					}
				}
				if (bfs_queue.empty()) break;
				cur_node = bfs_queue.front();
				bfs_queue.pop();
			} while (cur_node);
			return node_to_search;
		}
		std::unordered_set<entt::entity> GetObjectListInNode(int node_num) {
			return node_list_[array_size_].object_list;
		}
	public:
		void Init(const Vector<Dimension> world_size) {
			world_size_ = world_size;

			node_list_[0].area.Set( world_size_ * 0, world_size_ );
			BuildTree(1, 0);
		}
		bool    Frame()
		{

		}
		bool    Render()
		{

		}
		bool    Release() 
		{

		}
	private:
		const int delta[3][8] = { {0, 1, 0, 1, 0, 1, 0, 1 },
						{0, 0, 1, 1, 0, 0, 1, 1 },
						{0, 0, 0, 0, 1, 1, 1, 1 }
		};
	};
}