#include "stdafx.h"
#include "QuadTreeMgr.h"
#include "TimeMgr.h"
#include "SceneMgr.h"
#include "Character.h"

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

	casted_nodes_.clear();
	//NodeCasting(applied_camera->CreateFrontRay(), root_node_);
	ray_casted_nodes = casted_nodes_.size();
	UpdatePhysics();
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
}

void reality::QuadTreeMgr::UpdatePhysics()
{
	if (TM_DELTATIME > physics_timestep)
		return;

	static double delta = 0;
	delta += TM_DELTATIME;
	if (delta < physics_timestep)
		return;

	delta = 0.0f;

	for (auto& dynamic_capsule : dynamic_capsule_list)
	{
		player_capsule_pos = dynamic_capsule.second->capsule.base;

		int cal = 0;
		vector<SpaceNode*> nodes;
		ObjectQueryByCapsule(dynamic_capsule.second->capsule, root_node_, nodes);

		if (nodes.empty())
			break;

		including_nodes_num.clear();

		CapsuleCallback result;
		for (auto node : nodes)
		{
			including_nodes_num.insert(node->node_num);
			for (auto& tri : node->static_triangles)
			{
				cal++;
				result = CapsuleToTriangle(dynamic_capsule.second->capsule, tri);
			
				if (result.reaction != CapsuleCallback::NONE)				
					break;
				
			}
			if (result.reaction != CapsuleCallback::NONE)
				break;
		}		
		if (result.reaction == CapsuleCallback::NONE)
		{
			SCENE_MGR->GetActor<Character>(dynamic_capsule.first)->GravityFall(9.81f);
		}
		else
		{
			SCENE_MGR->GetActor<Character>(dynamic_capsule.first)->GetMovementComponent()->gravity = 0.0f;
		}

		SCENE_MGR->GetActor<Character>(dynamic_capsule.first)->capsule_callback = result;

		calculating_triagnles = cal;
		nodes.clear();
	}
}

void reality::QuadTreeMgr::NodeCasting(RayShape& ray, SpaceNode* node)
{
	if (FrustumToAABB(camera_frustum_, node->area) == CollideType::OUTSIDE)
		return;
	
	if (RayToAABB(ray, node->area))
	{
		if (node->is_leaf && node->static_triangles.size() > 0)
		{
			float dist = Distance(node->area.center, ray.start);
			casted_nodes_.insert(make_pair(dist, node));
			return;
		}
		if (!node->is_leaf)
		{
			NodeCasting(ray, node->child_node_[0]);
			NodeCasting(ray, node->child_node_[1]);
			NodeCasting(ray, node->child_node_[2]);
			NodeCasting(ray, node->child_node_[3]);
		}
	}
}

void reality::QuadTreeMgr::ObjectQueryByCapsule(CapsuleShape& capsule, SpaceNode* node, vector<SpaceNode*>& node_list)
{
	auto result = AABBToCapsule(node->area, capsule);
	if (result)
	{
		if (node->is_leaf)
		{
			node_list.push_back(node);
			return;			
		}
		if (!node->is_leaf)
		{
			ObjectQueryByCapsule(capsule, node->child_node_[0], node_list);
			ObjectQueryByCapsule(capsule, node->child_node_[1], node_list);
			ObjectQueryByCapsule(capsule, node->child_node_[2], node_list);
			ObjectQueryByCapsule(capsule, node->child_node_[3], node_list);
		}
	} 
}

RayCallback reality::QuadTreeMgr::RaycastAdjustLevel(RayShape& ray, float max_distance)
{
	const float ts = 1.0f / 20.0f;
	static float acc = 0.0f;
	acc += TM_DELTATIME;
	if (acc < ts)
		return RayCallback();

	acc = 0.0f;

	map<float, RayCallback> callback_list;

	int cal = 0;
	for (auto& node : casted_nodes_)
	{
		if (node.first > max_distance)
			break;

		for (auto& tri : node.second->static_triangles)
		{
			cal++;
			auto& callback = RayToTriangle(ray, tri);
			if (callback.success)
			{
				callback_list.insert(make_pair(callback.distance, callback));
			}
		}
	}
	cal = 0;

	if (callback_list.begin() == callback_list.end())
		return RayCallback();

	return callback_list.begin()->second;
}

void reality::QuadTreeMgr::RegisterDynamicCapsule(entt::entity ent)
{	
	if (dynamic_capsule_list.find(ent) != dynamic_capsule_list.end())
		return;

	C_CapsuleCollision* actor_capsule = SCENE_MGR->GetRegistry().try_get<C_CapsuleCollision>(ent);
	if (actor_capsule)
	{
		dynamic_capsule_list.insert(make_pair(ent, actor_capsule));
	}
}
