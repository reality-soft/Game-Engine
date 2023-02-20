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

	XMVECTOR min = XMVectorSet(vertices[coner_index[2]].p.x, MIN_HEIGHT, vertices[coner_index[2]].p.z, 0);
	XMVECTOR max = XMVectorSet(vertices[coner_index[1]].p.x, MAX_HEIGHT, vertices[coner_index[1]].p.z, 0);
	area = AABBShape(min, max);

	UINT row_cells = coner_index[1] - coner_index[0];
	UINT col_cells = (coner_index[2] - coner_index[0]) / world_col;
	UINT total_cells = row_cells * col_cells;
	index_list.resize(total_cells * 6);

	UINT index = 0;
	for (int col = 0; col < col_cells; ++col)
	{
		for (int row = 0; row < row_cells; ++row)
		{
			index_list[index + 0] = coner_index[0] + row + (col * world_col);
			index_list[index + 1] = index_list[index + 0] + 1;
			index_list[index + 2] = coner_index[0] + row + ((col + 1) * world_col);

			index_list[index + 3] = index_list[index + 2];
			index_list[index + 4] = index_list[index + 1];
			index_list[index + 5] = index_list[index + 2] + 1;

			index += 6;
		}
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

SpaceNode* KGCA41B::QuadTreeMgr::BuildTree(UINT depth, int row1, int col1, int row2, int col2)
{
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

		new_node->child_node_[0] = shared_ptr<SpaceNode>(BuildTree(depth + 1, row1,    col1,	row_mid, col_mid));
		new_node->child_node_[1] = shared_ptr<SpaceNode>(BuildTree(depth + 1, row_mid, col1,    row2,    col_mid));
		new_node->child_node_[2] = shared_ptr<SpaceNode>(BuildTree(depth + 1, row1,    col_mid, row_mid, col2));
		new_node->child_node_[3] = shared_ptr<SpaceNode>(BuildTree(depth + 1, row_mid, col_mid, row2,    col2));
	}

	return new_node;
}

void KGCA41B::QuadTreeMgr::RenderNode(SpaceNode* node_to_render)
{
	DX11APP->GetDeviceContext()->IASetIndexBuffer(node_to_render->index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	DX11APP->GetDeviceContext()->DrawIndexed(node_to_render->index_list.size(), 0, 0);
}

int KGCA41B::QuadTreeMgr::UpdateNodeObjectBelongs(int cur_node_num, const AABBShape& object_area, entt::entity object_id)
{
	SpaceNode* parent_node = root_node_.get();
	int new_node_num = 0;

	for (int i = 0; i < 4; ++i)
	{
		SpaceNode* child_node = parent_node->child_node_[i].get();

		if (child_node->area.AABBOverlap(object_area) == OverlapType::INSIDE)
		{
			if (child_node->node_depth == max_depth)
			{
				new_node_num = child_node->node_num;
				break;
			}

			i = -1;
			parent_node = child_node;
		}
		else if (child_node->area.AABBOverlap(object_area) == OverlapType::INTERSECT)
		{
			new_node_num = parent_node->node_num;
			break;
		}
	}
	if (new_node_num == cur_node_num)
	{
		return cur_node_num;
	}

	total_nodes_[cur_node_num].get()->object_list.erase(object_id);
	total_nodes_[new_node_num].get()->object_list.insert(object_id);

	return new_node_num;
}

std::vector<int> KGCA41B::QuadTreeMgr::FindCollisionSearchNode(int node_num)
{
	std::vector<int> node_to_search;
	SpaceNode* current_node = total_nodes_[node_num].get();

	int last_child_num = 0;

	for (int i = 0; i < max_depth - current_node->node_depth; ++i)
	{
		last_child_num += (int)pow(max_depth, i + 1);
	}

	for (int n = node_num; n <= last_child_num; ++n)
	{
		node_to_search.push_back(n);
	}

	return node_to_search;
}

std::unordered_set<entt::entity> KGCA41B::QuadTreeMgr::GetObjectListInNode(int node_num)
{
	return total_nodes_[node_num].get()->object_list;
}

void KGCA41B::QuadTreeMgr::Init(Level* level_to_devide, int _max_depth)
{
	deviding_level_ = shared_ptr<Level>(level_to_devide);

	world_size_ = level_to_devide->GetWorldSize();
	max_depth = _max_depth;

	root_node_ = shared_ptr<SpaceNode>(BuildTree(0, 0,0, world_size_.x, world_size_.y));

	root_node_.get()->index_list;
}

void KGCA41B::QuadTreeMgr::Frame(CameraSystem* applied_camera)
{
	camera_frustum_ = Frustum(applied_camera->GetViewProj());
	deviding_level_.get()->Update();

}
void KGCA41B::QuadTreeMgr::Render()
{
	MapCulling(camera_frustum_, root_node_.get());
}

void KGCA41B::QuadTreeMgr::Release()
{

}

void KGCA41B::QuadTreeMgr::MapCulling(Frustum& frustum, SpaceNode* node)
{
	OverlapType result = frustum.AABBOverlap(node->area);

	deviding_level_.get()->Render(true);

	if (result == OverlapType::INSIDE)
	{
		RenderNode(node);
	}
	else if (result == OverlapType::INTERSECT)
	{
		if (node->node_depth == max_depth)
		{
			RenderNode(node);
		}
		else
		{
			for (int i = 0; i < 4; ++i)
			{
				MapCulling(frustum, node->child_node_[i].get());
			}
		}
	}
}

void KGCA41B::QuadTreeMgr::ObjectCulling()
{
}