#include "stdafx.h"
#include "QuadTreeMgr.h"

using namespace KGCA41B;

KGCA41B::SpaceNode::SpaceNode(UINT num, UINT depth)
{
	node_num = num;
	node_depth = depth;
}

void KGCA41B::SpaceNode::SetNode(Level* level)
{
	vector<Vertex> vertices = level->GetLevelVertex();
	vector<UINT> indices = level->GetLevelIndex();
	UINT world_row = level->GetWorldSize().x + 1;
	UINT world_col = level->GetWorldSize().y + 1;

	area.min = XMVectorSet(vertices[coner_index[2]].p.x, MIN_HEIGHT, vertices[coner_index[2]].p.z, 0);
	area.max = XMVectorSet(vertices[coner_index[1]].p.x, MAX_HEIGHT, vertices[coner_index[1]].p.z, 0);


	UINT row_cells = coner_index[1] - coner_index[0];
	UINT col_cells = (coner_index[2] - coner_index[0]) / world_col;
	UINT total_cells = row_cells * col_cells;
	index_list.resize(total_cells);

	UINT index = 0;
	for (int col = 0; col < col_cells; ++col)
	{
		for (int row = 0; row < row_cells; ++row)
		{
			index_list[index + 0] = coner_index[0] + col + (row * world_col);
			index_list[index + 1] = index_list[index + 0] + 1;
			index_list[index + 2] = coner_index[0] + col + ((row + 1) * world_col);

			index_list[index + 3] = index_list[index + 2];
			index_list[index + 4] = index_list[index + 1];
			index_list[index + 5] = index_list[index + 2] + 1;
		}
		index += 6;
	}

	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA subdata;
	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.ByteWidth = sizeof(UINT) * index_list.size();
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	subdata.pSysMem = index_list.data();

	HRESULT hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, index_buffer.GetAddressOf());
}

//int KGCA41B::SpacePartition::GetLeftMostChildOfNode(int depth, int node_num)
//{
//	if (node_num == 0) return 1;
//	int left_most_node = GetLeftMostChildAtDepth(depth);
//	int next_layer_left_most_node = GetLeftMostChildAtDepth(depth + 1);
//	int children_per_node = 1 << DIMENSION;
//	return next_layer_left_most_node + (node_num - left_most_node) * children_per_node;
//}
//
//int KGCA41B::SpacePartition::GetLeftMostChildAtDepth(int depth)
//{
//	int children_per_node = 1 << DIMENSION;
//	return ((1 << ((depth - 1) * DIMENSION)) - 1) / (children_per_node - 1);
//}
//
//int KGCA41B::SpacePartition::GetRightMostChildAtDepth(int depth)
//{
//	int children_per_node = 1 << DIMENSION;
//	return ((1 << (depth * DIMENSION)) - 1) / (children_per_node - 1) - 1;
//}

SpaceNode* KGCA41B::QuadTreeMgr::BuildTree(UINT depth, int row1, int col1, int row2, int col2)
{
	/*
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
	*/

	SpaceNode* new_node = new SpaceNode(node_count++, depth);

	new_node->coner_index[0] = col1 * (world_size_.x + 1) + row1;
	new_node->coner_index[1] = col1 * (world_size_.x + 1) + row2;
	new_node->coner_index[2] = col2 * (world_size_.x + 1) + row1;
	new_node->coner_index[3] = col2 * (world_size_.x + 1) + row2;

	new_node->SetNode(deviding_level_.get());

	total_nodes_.push_back(shared_ptr<SpaceNode>(new_node));

	if (depth < max_depth)
	{
		int row_mid = (row1 + row2) / 2;
		int col_mid = (col1 + col2) / 2;

		new_node->child_node_[0] = shared_ptr<SpaceNode>(BuildTree(depth + 1, row1,    row_mid, col1,    col_mid));
		new_node->child_node_[1] = shared_ptr<SpaceNode>(BuildTree(depth + 1, row_mid, row2,    col1,    col_mid));
		new_node->child_node_[2] = shared_ptr<SpaceNode>(BuildTree(depth + 1, row1,    row_mid, col_mid, col2));
		new_node->child_node_[3] = shared_ptr<SpaceNode>(BuildTree(depth + 1, row_mid, row2,    col_mid, col2));
	}

	return new_node;
}

//int KGCA41B::SpacePartition::UpdateNodeObjectBelongs(int cur_node_num, const AABB<3>& object_area, entt::entity object_id)
//{
//	std::queue<int> bfs_queue;
//	int new_node = cur_node_num;
//	bfs_queue.push(new_node);
//	int children_per_node = 1 << DIMENSION;
//
//	AABB<DIMENSION> object_area_d;
//#if (DIMENSION == OCT_TREE)
//	object_area_d = object_area;
//#elif (DIMENSION == QUAD_TREE)
//	Vector<3> min_coord = object_area.MinCoord();
//	Vector<3> size = object_area.Size();
//	object_area_d.Set({ min_coord[0], min_coord[1] }, { size[0], size[1] });
//#endif
//	while (true) {
//		if (bfs_queue.empty()) break;
//		new_node = bfs_queue.front();
//		bfs_queue.pop();
//
//		int left_most_child = GetLeftMostChildOfNode(node_list_[new_node].depth, new_node);
//		for (int cur_child_node = left_most_child; cur_child_node < left_most_child + children_per_node; cur_child_node++) {
//			if (left_most_child >= array_size_) continue;
//
//			AABB<DIMENSION> cur_node_area = node_list_[cur_child_node].area;
//
//			if (Collision<DIMENSION>::CubeToCube(object_area_d, cur_node_area) == CollisionType::C_L_IN_R) {
//				bfs_queue.push(cur_child_node);
//				break;
//			}
//		}
//	} 
//
//	node_list_[cur_node_num].object_list.erase(object_id);
//	node_list_[new_node].object_list.insert(object_id);
//	return new_node;
//}

//std::vector<int> KGCA41B::SpacePartition::FindCollisionSearchNode(int node_num, AABB<3>& object_area)
//{
//	std::queue<int> bfs_queue;
//	std::vector<int> node_to_search;
//
//	AABB<DIMENSION> object_area_d;
//#if (DIMENSION == OCT_TREE)
//	object_area_d = object_area;
//#elif (DIMENSION == QUAD_TREE)
//	Vector<3> min_coord = object_area.MinCoord();
//	Vector<3> size = object_area.Size();
//	object_area_d.Set({ min_coord[0], min_coord[1] }, { size[0], size[1] });
//#endif
//
//	int cur_node = node_num;
//	int children_per_node = 1 << DIMENSION;
//	do {
//		int left_most_child = GetLeftMostChildOfNode(node_list_[cur_node].depth, cur_node);
//		for (int cur_child = left_most_child; cur_child < left_most_child + children_per_node; cur_child++) {
//			if (left_most_child >= array_size_) continue;
//
//			AABB<DIMENSION> cur_child_node_area = node_list_[cur_child].area;
//
//			if (Collision<DIMENSION>::CubeToCube(object_area_d, cur_child_node_area) != CollisionType::C_OUT) {
//				bfs_queue.push(cur_child);
//				node_to_search.push_back(cur_child);
//			}
//		}
//		if (bfs_queue.empty()) break;
//		cur_node = bfs_queue.front();
//		bfs_queue.pop();
//	} while (cur_node);
//	return node_to_search;
//}

//std::unordered_set<entt::entity> KGCA41B::SpacePartition::GetObjectListInNode(int node_num)
//{
//	return node_list_[node_num].object_list;
//}
//
void KGCA41B::QuadTreeMgr::Init(Level* level_to_devide, Camera* camera_to_apply, int _max_depth)
{
	deviding_level_ = shared_ptr<Level>(level_to_devide);
	apllied_camera_ = shared_ptr<Camera>(camera_to_apply);

	//world_size_.left   = -deviding_level_.get()->GetWorldSize().x / 2;
	//world_size_.right  =  deviding_level_.get()->GetWorldSize().x / 2;
	//world_size_.top    =  deviding_level_.get()->GetWorldSize().y / 2;
	//world_size_.bottom = -deviding_level_.get()->GetWorldSize().y / 2;

	world_size_ = level_to_devide->GetWorldSize();
	max_depth = _max_depth;

	root_node_ = shared_ptr<SpaceNode>(BuildTree(0, 0,0, world_size_.x, world_size_.y));

	root_node_.get()->index_list;

	total_nodes_.size();
}

bool KGCA41B::QuadTreeMgr::Frame()
{
	return true;
}

bool KGCA41B::QuadTreeMgr::Render()
{
	return true;
}

bool KGCA41B::QuadTreeMgr::Release()
{
	return true;
}