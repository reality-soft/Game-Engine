#include "stdafx.h"
#include "QuadTreeMgr.h"
#include "TimeMgr.h"

using namespace reality;

reality::SpaceNode::SpaceNode(UINT num, UINT depth)
{
	node_num = num;
	node_depth = depth;
}

reality::SpaceNode::~SpaceNode()
{
	for (int i = 0; i < 4; ++i)
	{
		if (child_node_[i])
		{
			delete child_node_[i];
			child_node_[i] = nullptr;
		}
	}
}

void reality::SpaceNode::SetNode(float min_x, float min_z, float max_x, float max_z)
{
	XMVECTOR min = XMVectorSet(min_x, MIN_HEIGHT, min_z, 0);
	XMVECTOR max = XMVectorSet(max_x, MAX_HEIGHT, max_z, 0);
	area = AABBShape(min, max);
}

void reality::QuadTreeMgr::Init(LightMeshLevel* level_to_devide, int max_depth)
{
	deviding_level_ = level_to_devide;

	float min_x = 0, min_z = 0;
	float max_x = 0, max_z = 0;
	for (auto& tri : level_to_devide->level_triangles)
	{
		min_x = min(min_x, XMVectorGetX(tri.vertex0));
		min_z = min(min_z, XMVectorGetZ(tri.vertex0));

		max_x = max(max_x, XMVectorGetX(tri.vertex0));
		max_z = max(max_z, XMVectorGetZ(tri.vertex0));
	}
	this->max_depth = max_depth;
	root_node_ = BuildTree(0, min_x, min_z, max_x, max_z);
}


SpaceNode* reality::QuadTreeMgr::BuildTree(UINT depth, float min_x, float min_z, float max_x, float max_z)
{
	SpaceNode* new_node = new SpaceNode(node_count++, depth);

	new_node->SetNode(min_x, min_z, max_x, max_z);
	total_nodes_.push_back(new_node);

	if (new_node->node_depth == max_depth)
	{
		new_node->is_leaf = true;
		SetStaticTriangles(new_node);
		if (new_node->static_triangles.size() > 0)
			leaf_nodes_.push_back(new_node);
	}

	if (depth < max_depth)
	{
		float row_mid = (min_x + max_x) / 2;
		float col_mid = (min_z + max_z) / 2;

		new_node->child_node_[0] = BuildTree(depth + 1, min_x, min_z,	row_mid, col_mid);
		new_node->child_node_[1] = BuildTree(depth + 1, row_mid, min_z, max_x,    col_mid);
		new_node->child_node_[2] = BuildTree(depth + 1, min_x,    col_mid, row_mid, max_z);
		new_node->child_node_[3] = BuildTree(depth + 1, row_mid, col_mid, max_x, max_z);
	}

	return new_node;
}

void reality::QuadTreeMgr::SetStaticTriangles(SpaceNode* node)
{
	for (auto& tri : deviding_level_->level_triangles)
	{
		CollideType result = AABBToTriagnle(node->area, tri);
		if (result != CollideType::OUTSIDE)
			node->static_triangles.push_back(tri);
	}

}

int reality::QuadTreeMgr::UpdateNodeObjectBelongs(int cur_node_num, const AABBShape& object_area, entt::entity object_id)
{
	SpaceNode* parent_node = root_node_;
	int new_node_num = 0;

	for (int i = 0; i < 4; ++i)
	{
		SpaceNode* child_node = parent_node->child_node_[i];

		//if (child_node->area.AABBOverlap(object_area) == OverlapType::INSIDE)
		//{
		//	if (child_node->node_depth == max_depth)
		//	{
		//		new_node_num = child_node->node_num;
		//		break;
		//	}

		//	i = -1;
		//	parent_node = child_node;
		//}
		//else if (child_node->area.AABBOverlap(object_area) == OverlapType::INTERSECT)
		//{
		//	new_node_num = parent_node->node_num;
		//	break;
		//}
	}
	if (new_node_num == cur_node_num)
	{
		return cur_node_num;
	}

	total_nodes_[cur_node_num]->object_list.erase(object_id);
	total_nodes_[new_node_num]->object_list.insert(object_id);

	return new_node_num;
}

std::vector<int> reality::QuadTreeMgr::FindCollisionSearchNode(int node_num)
{
	std::vector<int> node_to_search;
	SpaceNode* current_node = total_nodes_[node_num];

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

std::unordered_set<entt::entity> reality::QuadTreeMgr::GetObjectListInNode(int node_num)
{
	return total_nodes_[node_num]->object_list;
}

void reality::QuadTreeMgr::Frame(CameraSystem* applied_camera)
{
	camera_frustum_ = Frustum(applied_camera->GetViewProj());
	deviding_level_->Update();
}

void reality::QuadTreeMgr::Release()
{
	if (root_node_)
	{
		delete root_node_;
		root_node_ = nullptr;
	}
	total_nodes_.clear();
	leaf_nodes_.clear();
	deviding_level_ = nullptr;
}

void reality::QuadTreeMgr::UpdatePhysics(float time_step)
{
	static float delta = 0;
	delta += TM_DELTATIME;
	if (delta < time_step)
		return;

	delta = 0.0f;
}

void reality::QuadTreeMgr::NodeCulling(SpaceNode* node)
{

}

void reality::QuadTreeMgr::ObjectCulling()
{

}

RayCallback reality::QuadTreeMgr::Raycast(RayShape& ray)
{
	map<float, RayCallback> callback_list;

	for (auto& nodes : leaf_nodes_)
	{
		int calculated = 0;
		if (RayToAABB(ray, nodes->area))
		{
			for (auto& tri : nodes->static_triangles)
			{
				calculated++;
				auto& callback = RayToTriangle(ray, tri);
				if (callback.success)
				{
					callback_list.insert(make_pair(callback.distance, callback));
				}
			}
		}
		calculated = 0;
	}



	if (callback_list.begin() == callback_list.end())
		return RayCallback();

	return callback_list.begin()->second;
}
