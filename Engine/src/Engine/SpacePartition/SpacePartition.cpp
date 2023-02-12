#include "SpacePartition.h"

int KGCA41B::SpacePartition::GetLeftMostChildOfNode(int depth, int node_num)
{
	if (node_num == 0) return 1;
	int left_most_node = GetLeftMostChildAtDepth(depth);
	int next_layer_left_most_node = GetLeftMostChildAtDepth(depth + 1);
	int children_per_node = 1 << DIMENSION;
	return next_layer_left_most_node + (node_num - left_most_node) * children_per_node;
}

int KGCA41B::SpacePartition::GetLeftMostChildAtDepth(int depth)
{
	int children_per_node = 1 << DIMENSION;
	return ((1 << ((depth - 1) * DIMENSION)) - 1) / (children_per_node - 1);
}

int KGCA41B::SpacePartition::GetRightMostChildAtDepth(int depth)
{
	int children_per_node = 1 << DIMENSION;
	return ((1 << (depth * DIMENSION)) - 1) / (children_per_node - 1) - 1;
}

void KGCA41B::SpacePartition::BuildTree(int depth, int node_num)
{
	if (depth >= MAX_DEPTH) return;
	int left_most_child = GetLeftMostChildOfNode(depth, node_num);
	Vector<DIMENSION> node_position;
	Vector<DIMENSION> node_size = node_list_[node_num].area.Size() / 2;
	int children_per_node = 1 << DIMENSION;
	for (int cur_child_node = left_most_child; cur_child_node < left_most_child + children_per_node; cur_child_node++) {
		for (int j = 0; j < DIMENSION; j++) {
			node_position[j] = node_list_[node_num].area.MinCoord()[j] + delta[j][cur_child_node - left_most_child] * node_size[j];
		}
		node_list_[cur_child_node].area.Set(node_position, node_size);
		node_list_[cur_child_node].depth = depth;
		BuildTree(depth + 1, cur_child_node);
	}
}

int KGCA41B::SpacePartition::FindNodeObjectBelongs(int node_num, const AABB<3>& object_area)
{
	std::queue<int> bfs_queue;
	int cur_node = node_num;
	int children_per_node = 1 << DIMENSION;

	AABB<DIMENSION> object_area_d;
#if (DIMENSION == OCT_TREE)
	object_area_d = object_area;
#elif (DIMENSION == QUAD_TREE)
	Vector<3> center_coord = object_area.CenterCoord();
	Vector<3> size = object_area.Size();
	object_area_d.Set({ center_coord[0], center_coord[1] }, { size[0], size[1] });
#endif
	do {
		int left_most_child = GetLeftMostChildOfNode(node_list_[cur_node].depth, cur_node);
		for (int cur_child_node = left_most_child; cur_child_node < left_most_child + children_per_node; cur_child_node++) {
			if (left_most_child >= array_size_) continue;

			AABB<DIMENSION> cur_node_area = node_list_[cur_child_node].area;

			if (Collision<DIMENSION>::CubeToCube(object_area_d, cur_node_area) == CollisionType::C_R_IN_L) {
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

std::vector<int> KGCA41B::SpacePartition::FindCollisionSearchNode(int node_num, AABB<3>& object_area)
{
	std::queue<int> bfs_queue;
	std::vector<int> node_to_search;

	AABB<DIMENSION> object_area_d;
#if (DIMENSION == OCT_TREE)
	object_area_d = object_area;
#elif (DIMENSION == QUAD_TREE)
	Vector<3> center_coord = object_area.CenterCoord();
	Vector<3> size = object_area.Size();
	object_area_d.Set({ center_coord[0], center_coord[1] }, { size[0], size[1] });
#endif

	int cur_node = node_num;
	int children_per_node = 1 << DIMENSION;
	do {
		int left_most_child = GetLeftMostChildOfNode(node_list_[cur_node].depth, cur_node);
		for (int cur_child = left_most_child; cur_child < left_most_child + children_per_node; cur_child++) {
			if (left_most_child >= array_size_) continue;

			AABB<DIMENSION> cur_child_node_area = node_list_[cur_child].area;

			if (Collision<DIMENSION>::CubeToCube(object_area_d, cur_child_node_area) != CollisionType::C_OUT) {
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

std::unordered_set<entt::entity> KGCA41B::SpacePartition::GetObjectListInNode(int node_num)
{
	return node_list_[array_size_].object_list;
}

void KGCA41B::SpacePartition::Init(const Vector<DIMENSION> world_size)
{
	world_size_ = world_size;

	node_list_[0].area.Set(world_size_ * 0, world_size_);
	BuildTree(1, 0);
}

bool KGCA41B::SpacePartition::Frame()
{
	return true;
}

bool KGCA41B::SpacePartition::Render()
{
	return true;
}

bool KGCA41B::SpacePartition::Release()
{
	return true;
}
