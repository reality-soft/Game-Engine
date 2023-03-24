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

	// build tree
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

	// set blocking field
	for (auto& guide_line : deviding_level_->GetGuideLines(GuideLine::GuideType::eBlocking))
	{
		if (guide_line.guide_type_ == GuideLine::GuideType::eBlocking)
		{
			for (UINT i = 1; i < guide_line.line_nodes.size(); ++i)
			{
				RayShape blocking_line;
				blocking_line.start = guide_line.line_nodes.at(i - 1);
				blocking_line.end = guide_line.line_nodes.at(i);

				blocking_line.start.m128_f32[1] = 0.0f;
				blocking_line.end.m128_f32[1] = 0.0f;

				blocking_lines.push_back(blocking_line);
			}
		}
	}

	// regist dynamic capsule
	const auto& capsule_view = SCENE_MGR->GetRegistry().view<C_CapsuleCollision>();
	for (auto& ent : capsule_view)
	{
		auto capsule_collision = SCENE_MGR->GetRegistry().try_get<C_CapsuleCollision>(ent);
		dynamic_capsule_list.insert(make_pair(ent, capsule_collision));
	}
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

void reality::QuadTreeMgr::Frame(CameraSystem* applied_camera)
{
	camera_frustum_ = Frustum(applied_camera->GetViewProj());
	UpdateCapsules();

	casted_nodes_.clear();
	NodeCasting(applied_camera->CreateFrontRay(), root_node_);
	ray_casted_nodes = casted_nodes_.size();
	UpdatePhysics();

	player_capsule_pos = SCENE_MGR->GetRegistry().try_get<C_CapsuleCollision>(SCENE_MGR->GetPlayer<Character>(0)->GetEntityId())->capsule.base;
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
	int a = 0;
	for (auto& dynamic_capsule : dynamic_capsule_list)
	{
		vector<SpaceNode*> nodes;
		ObjectQueryByCapsule(dynamic_capsule.second->capsule, root_node_, nodes);

		if (nodes.empty())
			break;

		CheckTriangle(dynamic_capsule.first, dynamic_capsule.second->capsule, nodes);
		CheckBlockingLine(dynamic_capsule.first, dynamic_capsule.second->capsule);

		// Add Object to LeafNode
		for (auto node : nodes)
		{
			node->object_list.insert(dynamic_capsule.first);
		}

		nodes.clear();
	}
	int b = 0;
}

void reality::QuadTreeMgr::CheckTriangle(entt::entity ent, CapsuleShape& capsule, vector<SpaceNode*> nodes)
{
	map<float, CapsuleCallback> floor_list;
	vector<RayShape> wall_list;

	for (auto node : nodes)
	{
		including_nodes_num.insert(node->node_num);
		for (auto& tri : node->static_triangles)
		{
			auto result = CapsuleToTriangleEx(capsule, tri);
			if (result.reaction == CapsuleCallback::FLOOR)
				floor_list.insert(make_pair(XMVectorGetY(result.floor_pos), result));
			if (result.reaction == CapsuleCallback::WALL)
				wall_list.push_back(RayShape(tri.GetMinXZ(), tri.GetMaxXZ()));
				
		}
	}

	if (floor_list.empty())
	{
		SCENE_MGR->GetActor<Character>(ent)->GravityFall(9.81f);
		SCENE_MGR->GetActor<Character>(ent)->movement_state_ = MovementState::GRAVITY_FALL;
		SCENE_MGR->GetActor<Character>(ent)->floor_height = capsule.base.m128_f32[1];
	}
	else
	{
		SCENE_MGR->GetActor<Character>(ent)->GetMovementComponent()->gravity = 0.0f;
		SCENE_MGR->GetActor<Character>(ent)->movement_state_ = MovementState::STAND_ON_FLOOR;
		SCENE_MGR->GetActor<Character>(ent)->floor_height = floor_list.rbegin()->first;
	}

	if (wall_list.size() > 0)
	{
		SCENE_MGR->GetActor<Character>(ent)->blocking_walls_ = wall_list;
	}	
}

void reality::QuadTreeMgr::CheckBlockingLine(entt::entity ent, CapsuleShape& capsule)
{
	for (auto& dynamic_capsule : dynamic_capsule_list)
	{
		if (dynamic_capsule.second->tag == "Enemy")
			continue;

		XMVECTOR capsule_pos = dynamic_capsule.second->capsule.base;
		capsule_pos.m128_f32[1] = 0.0f;

		for (auto& blocking_line : blocking_lines)
		{
			float distance_from_line = XMVectorGetX(XMVector3LinePointDistance(blocking_line.start, blocking_line.end, capsule_pos));

			if (distance_from_line <= dynamic_capsule.second->capsule.radius)
			{
				XMVECTOR OB = blocking_line.end - blocking_line.start;
				XMVECTOR OA = dynamic_capsule.second->capsule.base - blocking_line.start;
				float dot = XMVectorGetX(XMVector3Dot(OA, OB));
				if (dot < 0)
				{
					OB *= -1.0f;
					OA = dynamic_capsule.second->capsule.base - blocking_line.end;
				}

				float proj_length = Vector3Length(Vector3Project(OB, OA));
				float line_length = Vector3Length(OB) + dynamic_capsule.second->capsule.radius;

				if (proj_length <= line_length)
				{
					SCENE_MGR->GetActor<Character>(dynamic_capsule.first)->blocking_walls_.push_back(blocking_line);
				}
			}
		}
	}
}

void reality::QuadTreeMgr::NodeCasting(const RayShape& ray, SpaceNode* node)
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

void reality::QuadTreeMgr::UpdateCapsules()
{
	vector<entt::entity> destroied_actors;

	for (auto& capsule : dynamic_capsule_list)
	{
		if (SCENE_MGR->GetActor<Actor>(capsule.first) == nullptr)
		{
			destroied_actors.push_back(capsule.first);
			capsule.second = nullptr;
		}
	}

	for (const auto& ent : destroied_actors)
	{
		dynamic_capsule_list.erase(ent);
	}
}

RayCallback reality::QuadTreeMgr::RaycastAdjustLevel(const RayShape& ray, float max_distance)
{
	map<float, RayCallback> callback_list;

	int cal = 0;
	for (auto& node : casted_nodes_)
	{
		if (node.first > max_distance)
			break;

		for (auto& tri : node.second->static_triangles)
		{
			cal++;
			const auto& callback = RayToTriangle(ray, tri);
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

pair<RayCallback, entt::entity> reality::QuadTreeMgr::RaycastAdjustActor(const RayShape& ray)
{
	map<float, RayCallback> callback_list;

	entt::entity selected_entity = entt::null;

	auto level_callback = RaycastAdjustLevel(ray, 15000);
	for (auto& capsule : dynamic_capsule_list)
	{
		if (capsule.first == SCENE_MGR->GetPlayer<Character>(0)->GetEntityId())
			continue;

		const auto& capsule_callback = RayToCapsule(ray, capsule.second->capsule);
		if (capsule_callback.success)
		{
			callback_list.insert(make_pair(capsule_callback.distance, capsule_callback));
			selected_entity = capsule.first;
		}
	}

	if (callback_list.begin() == callback_list.end())
		return make_pair(RayCallback(), entt::null);

	if (level_callback.success && level_callback.distance < callback_list.begin()->first)
		return make_pair(RayCallback(), entt::null);

	return make_pair(callback_list.begin()->second, selected_entity);
}

void reality::QuadTreeMgr::RegistDynamicCapsule(entt::entity ent)
{
	auto capsule_collision = SCENE_MGR->GetRegistry().try_get<C_CapsuleCollision>(ent);
	if (capsule_collision != nullptr)
		dynamic_capsule_list.insert(make_pair(ent, capsule_collision));
}
