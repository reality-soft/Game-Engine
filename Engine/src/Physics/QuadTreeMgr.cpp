#include "stdafx.h"
#include "FileTransfer.h"
#include "QuadTreeMgr.h"
#include "TimeMgr.h"
#include "SceneMgr.h"

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
	XMFLOAT3 min = XMFLOAT3(min_x, MIN_HEIGHT, min_z);
	XMFLOAT3 max = XMFLOAT3(max_x, MAX_HEIGHT, max_z);
	area = AABBShape(min, max);
	culling_aabb.CreateFromPoints(culling_aabb, _XMVECTOR3(area.min), _XMVECTOR3(area.max));
}

void reality::SpaceNode::SetVisible(bool _visible)
{
	visible = _visible;

	if (!is_leaf)
	{
		child_node_[0]->SetVisible(_visible);
		child_node_[1]->SetVisible(_visible);
		child_node_[2]->SetVisible(_visible);
		child_node_[3]->SetVisible(_visible);
	}
}

void reality::QuadTreeMgr::Init(StaticMeshLevel* level_to_devide, entt::registry& reg)
{
	deviding_level_ = level_to_devide;
	registry_ = &reg;
}

void reality::QuadTreeMgr::CreateQuadTreeData(int max_depth)
{
	// build tree
	float min_x = 0, min_z = 0;
	float max_x = 0, max_z = 0;
	for (auto& tri : deviding_level_->level_triangles)
	{
		min_x = min(min_x, tri.vertex0.x);
		min_z = min(min_z, tri.vertex0.z);

		max_x = max(max_x, tri.vertex0.x);
		max_z = max(max_z, tri.vertex0.z);
	}
	this->max_depth = max_depth;
	node_count = 0;
	root_node_ = BuildPhysicsTree(0, min_x, min_z, max_x, max_z);
}

void reality::QuadTreeMgr::ImportQuadTreeData(string mapdat_file)
{
	FileTransfer read_file(mapdata_dir + mapdat_file, READ);

	UINT null_node_index = 99999;

	// physics tree data
	UINT physics_node_count;
	read_file.ReadBinary<UINT>(physics_node_count);

	for (UINT n = 0; n < physics_node_count; ++n)
	{
		// index data
		SpaceNode* new_node = new SpaceNode(n, 0);
		total_nodes_.insert(make_pair(n, new_node));

		UINT node_num; bool is_leaf;
		read_file.ReadBinary<UINT>(node_num);
		read_file.ReadBinary<bool>(is_leaf);

		new_node->node_num = node_num;
		new_node->is_leaf = is_leaf;

		UINT parent_node_num;
		read_file.ReadBinary<UINT>(parent_node_num);
		if (parent_node_num != null_node_index)
		{
			new_node->parent_node_index = parent_node_num;
		}
		else
		{
			new_node->parent_node_index = null_node_index;
		}

		UINT child_node_index[4] = { 0, };
		read_file.ReadBinary<UINT>(child_node_index[0]);
		read_file.ReadBinary<UINT>(child_node_index[1]);
		read_file.ReadBinary<UINT>(child_node_index[2]);
		read_file.ReadBinary<UINT>(child_node_index[3]);

		if (child_node_index[0] != null_node_index)
		{
			new_node->child_node_index[0] = child_node_index[0];
			new_node->child_node_index[1] = child_node_index[1];
			new_node->child_node_index[2] = child_node_index[2];
			new_node->child_node_index[3] = child_node_index[3];
		}
		else
		{
			new_node->child_node_index[0] = null_node_index;
			new_node->child_node_index[1] = null_node_index;
			new_node->child_node_index[2] = null_node_index;
			new_node->child_node_index[3] = null_node_index;
		}

		// aabb data
		XMFLOAT3 min, max, center;
		read_file.ReadBinary<XMFLOAT3>(min);
		read_file.ReadBinary<XMFLOAT3>(max);
		read_file.ReadBinary<XMFLOAT3>(center);

		new_node->area = AABBShape(min, max);

		// collision data
		UINT triangles_count = 0;
		read_file.ReadBinary<UINT>(triangles_count);

		for (UINT t = 0; t < triangles_count; ++t)
		{
			UINT tri_index;
			XMFLOAT3 normal, vertex0, vertex1, vertex2;

			read_file.ReadBinary<UINT>(tri_index);
			read_file.ReadBinary<XMFLOAT3>(normal);
			read_file.ReadBinary<XMFLOAT3>(vertex0);
			read_file.ReadBinary<XMFLOAT3>(vertex1);
			read_file.ReadBinary<XMFLOAT3>(vertex2);

			TriangleShape new_triangle(vertex0, vertex1, vertex2);
			new_triangle.index = tri_index;

			new_node->static_triangles.push_back(new_triangle);
		}
	}

	InitImported();
}

void reality::QuadTreeMgr::InitImported()
{
	for (auto& item : total_nodes_)
	{
		auto node = item.second;

		if (node->parent_node_index != 99999)
		{
			node->parent_node = total_nodes_.find(node->parent_node_index)->second;
		}
		else
		{
			root_node_ = node;
		}

		if (node->child_node_index[0] != 99999)
		{
			node->child_node_[0] = total_nodes_.find(node->child_node_index[0])->second;
			node->child_node_[1] = total_nodes_.find(node->child_node_index[1])->second;
			node->child_node_[2] = total_nodes_.find(node->child_node_index[2])->second;
			node->child_node_[3] = total_nodes_.find(node->child_node_index[3])->second;
		}

		if (node->is_leaf)
			leaf_nodes_.insert(item);

		node->culling_aabb.CreateFromPoints(node->culling_aabb, _XMVECTOR3(node->area.min), _XMVECTOR3(node->area.max));
	}

	for (const auto& tri : deviding_level_->level_triangles)
	{
		if (tri.index == 11102 || tri.index == 11103 ||
			tri.index == 11104 || tri.index == 11105 ||
			tri.index == 11106 || tri.index == 11107 ||
			tri.index == 11108 || tri.index == 11109)
			car_triagnles_.push_back(tri);
	}
}

void reality::QuadTreeMgr::ImportGuideLines(string mapdat_file, GuideType guide_type)
{
	FileTransfer out_mapdata(mapdata_dir + mapdat_file, READ);

	vector<GuideLine> new_guide_lines_;

	UINT num_guide_lines = 0;
	out_mapdata.ReadBinary<UINT>(num_guide_lines);
	for (UINT i = 0; i < num_guide_lines; ++i)
	{
		GuideLine new_guide_line;
		new_guide_line.guide_type_ = guide_type;

		UINT num_nodes = 0;
		out_mapdata.ReadBinary<UINT>(num_nodes);

		for (UINT j = 0; j < num_nodes; ++j)
		{
			UINT node_number;
			XMVECTOR node_pos;
			out_mapdata.ReadBinary<UINT>(node_number);
			out_mapdata.ReadBinary<XMVECTOR>(node_pos);

			new_guide_line.AddNode(node_pos);
		}

		new_guide_lines_.push_back(new_guide_line);
	}

	auto name = split(mapdat_file, '.');
	guide_lines_.insert(make_pair(name[0], new_guide_lines_));
	new_guide_lines_.clear();
}

vector<GuideLine>* reality::QuadTreeMgr::GetGuideLines(string name)
{
	if (guide_lines_.find(name) != guide_lines_.end())
		return &guide_lines_.find(name)->second;
}

vector<TriangleShape>& reality::QuadTreeMgr::GetCarTriangles()
{
	return car_triagnles_;
}

void reality::QuadTreeMgr::SetBlockingFields(string name)
{
	vector<GuideLine>* guide_lines_ = GetGuideLines(name);
	for (const auto& guide_line : *guide_lines_)
	{
		if (guide_line.guide_type_ == GuideType::eBlocking)
		{
			for (UINT i = 1; i < guide_line.line_nodes.size(); ++i)
			{
				RayShape ray;
				ray.start = _XMFLOAT3(guide_line.line_nodes.at(i - 1));
				ray.end = _XMFLOAT3(guide_line.line_nodes.at(i));
				blocking_fields_.push_back(ray);
			}			
		}
	}
}

SpaceNode* reality::QuadTreeMgr::BuildPhysicsTree(UINT depth, float min_x, float min_z, float max_x, float max_z)
{
	SpaceNode* new_node = new SpaceNode(node_count++, depth);

	new_node->SetNode(min_x, min_z, max_x, max_z);
	total_nodes_.insert(make_pair(new_node->node_num, new_node));

	if (new_node->node_depth == max_depth)
	{
		new_node->is_leaf = true;
		SetStaticTriangles(new_node);
		if (new_node->static_triangles.size() > 0)
			leaf_nodes_.insert(make_pair(new_node->node_num, new_node));
	}

	if (depth < max_depth)
	{
		float row_mid = (min_x + max_x) / 2;
		float col_mid = (min_z + max_z) / 2;

		new_node->child_node_[0] = BuildPhysicsTree(depth + 1, min_x, min_z,	row_mid, col_mid);
		new_node->child_node_[0]->parent_node = new_node;

		new_node->child_node_[1] = BuildPhysicsTree(depth + 1, row_mid, min_z, max_x,    col_mid);
		new_node->child_node_[1]->parent_node = new_node;

		new_node->child_node_[2] = BuildPhysicsTree(depth + 1, min_x,    col_mid, row_mid, max_z);
		new_node->child_node_[2]->parent_node = new_node;

		new_node->child_node_[3] = BuildPhysicsTree(depth + 1, row_mid, col_mid, max_x, max_z);
		new_node->child_node_[3]->parent_node = new_node;
	}

	return new_node;
}

void reality::QuadTreeMgr::SetStaticTriangles(SpaceNode* node)
{
	for (const auto& tri : deviding_level_->level_triangles)
	{
		XMVECTOR Normal = XMVector3Cross(XMVectorSubtract(_XMVECTOR3(tri.vertex1), _XMVECTOR3(tri.vertex0)), XMVectorSubtract(_XMVECTOR3(tri.vertex2), _XMVECTOR3(tri.vertex0)));
		if (XMVector3Equal(Normal, XMVectorZero()))
			continue;

		bool tri_aabb_intersect = node->culling_aabb.Intersects(_XMVECTOR3(tri.vertex0), _XMVECTOR3(tri.vertex1), _XMVECTOR3(tri.vertex2));
		if (tri_aabb_intersect)
			node->static_triangles.push_back(tri);
	}
}

void reality::QuadTreeMgr::AddStaticTriangles(const vector<TriangleShape>& triangles)
{
	for (auto& item : leaf_nodes_)
	{
		auto node = item.second;
		for (const auto& tri : triangles)
		{
			XMVECTOR Normal = XMVector3Cross(XMVectorSubtract(_XMVECTOR3(tri.vertex1), _XMVECTOR3(tri.vertex0)), XMVectorSubtract(_XMVECTOR3(tri.vertex2), _XMVECTOR3(tri.vertex0)));
			if (XMVector3Equal(Normal, XMVectorZero()))
				continue;

			bool tri_aabb_intersect = node->culling_aabb.Intersects(_XMVECTOR3(tri.vertex0), _XMVECTOR3(tri.vertex1), _XMVECTOR3(tri.vertex2));
			if (tri_aabb_intersect)
				node->static_triangles.push_back(tri);
		}
	}
}

void reality::QuadTreeMgr::Frame(CameraSystem* applied_camera)
{
	camera_frustum_.CreateFromMatrix(camera_frustum_, applied_camera->projection_matrix);
	camera_frustum_.Transform(camera_frustum_, applied_camera->world_matrix);

	visible_nodes = 0;
	NodeCulling(root_node_);

	XMFLOAT2 camera_pitch_yaw = applied_camera->GetCamera()->pitch_yaw;
	camera_rotation_ = XMMatrixRotationRollPitchYaw(camera_pitch_yaw.x, camera_pitch_yaw.y, 0);	
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
	capsule_stbuffer.elements.clear();
	sphere_stbuffer.elements.clear();

	capsule_stbuffer.elements.resize(32);
	sphere_stbuffer.elements.resize(32);

	// Capsule Node Update // 

	UINT capsule_index = 0;
	const auto& c_capsule_view = registry_->view<C_CapsuleCollision>();

	for (const auto& ent : c_capsule_view)
	{
		if (capsule_index >= capsule_stbuffer.elements.size())
			break;

		auto& c_capsule = registry_->get<C_CapsuleCollision>(ent);

		CapsuleImpulse(ent, c_capsule);

		SpaceNode* query_start = total_nodes_.find(c_capsule.enclosed_node_index)->second;
		query_start = ParentNodeQuery(c_capsule, query_start);

		vector<SpaceNode*> queried_nodes;
		LeafNodeQuery(c_capsule, query_start, queried_nodes);

		if (queried_nodes.empty())
			continue;

		auto capsule_info = GetTipBaseAB(c_capsule.capsule);
		capsule_stbuffer.elements[capsule_index].point_a = _XMFLOAT3(capsule_info[2]);
		capsule_stbuffer.elements[capsule_index].point_b = _XMFLOAT3(capsule_info[3]);
		capsule_stbuffer.elements[capsule_index].radius = c_capsule.capsule.radius;
		capsule_stbuffer.elements[capsule_index].entity = (int)ent;

		for (int i = 0; i < 4; ++i)
		{
			if (i >= queried_nodes.size())
				capsule_stbuffer.elements[capsule_index].node_numbers[i] = 0;

			else
				capsule_stbuffer.elements[capsule_index].node_numbers[i] = queried_nodes[i]->node_num;
		}

		auto actor = SCENE_MGR->GetActor<Actor>(ent);
		if (actor)
			actor->visible = queried_nodes[0]->visible;

		capsule_index++;
	}

	// Sphere Node Update // 

	UINT sphere_index = 0;
	const auto& c_sphere_view = registry_->view<C_SphereCollision>();

	for (const auto& ent : c_sphere_view)
	{
		if (sphere_index >= sphere_stbuffer.elements.size())
			break;

		auto& c_sphere = registry_->get<C_SphereCollision>(ent);

		SpaceNode* query_start = total_nodes_.find(c_sphere.enclosed_node_index)->second;
		query_start = ParentNodeQuery(c_sphere, query_start);

		vector<SpaceNode*> queried_nodes;
		LeafNodeQuery(c_sphere, query_start, queried_nodes);

		if (queried_nodes.empty())
			continue;

		sphere_stbuffer.elements[sphere_index].center = c_sphere.sphere.center;
		sphere_stbuffer.elements[sphere_index].radius = c_sphere.sphere.radius;
		sphere_stbuffer.elements[sphere_index].entity = (int)ent;

		for (int i = 0; i < 4; ++i)
		{
			if (i >= queried_nodes.size())
				sphere_stbuffer.elements[sphere_index].node_numbers[i] = 0;

			else
				sphere_stbuffer.elements[sphere_index].node_numbers[i] = queried_nodes[i]->node_num;
		}

		auto actor = SCENE_MGR->GetActor<Actor>(ent);
		if (actor)
			actor->visible = queried_nodes[0]->visible;

		sphere_index++;
	}

	DX11APP->GetDeviceContext()->UpdateSubresource(capsule_stbuffer.buffer.Get(), 0, 0, capsule_stbuffer.elements.data(), 0, 0);
	DX11APP->GetDeviceContext()->UpdateSubresource(sphere_stbuffer.buffer.Get(), 0, 0, sphere_stbuffer.elements.data(), 0, 0);

	RunPhysicsCS("PhysicsCS.cso");
	MovementByPhysicsCS();
	SphereUpdateByPhysicsCS();
}

void reality::QuadTreeMgr::NodeCulling(SpaceNode* node)
{
	auto result = camera_frustum_.Contains(node->culling_aabb);

	if (result == ContainmentType::CONTAINS)
	{
		node->SetVisible(true);
		++visible_nodes;
	}
	else if (result == ContainmentType::INTERSECTS)
	{
		if (node->is_leaf)
		{
			node->SetVisible(true);
			++visible_nodes;
		}
		else
		{
			NodeCulling(node->child_node_[0]);
			NodeCulling(node->child_node_[1]);
			NodeCulling(node->child_node_[2]);
			NodeCulling(node->child_node_[3]);
		}
	}
	else
	{
		node->SetVisible(false);
	}
}

SpaceNode* reality::QuadTreeMgr::ParentNodeQuery(C_CapsuleCollision& c_capsule, SpaceNode* node)
{
	if (node == nullptr)
		return root_node_;

	auto result = CapsuleToAABB(node->area, c_capsule.capsule);

	if (result == CollideType::INSIDE)
		return node;

	else
		return ParentNodeQuery(c_capsule, node->parent_node);
}

SpaceNode* reality::QuadTreeMgr::ParentNodeQuery(C_SphereCollision& c_sphere, SpaceNode* node)
{
	if (node == nullptr)
		return root_node_;

	auto result = SphereToAABB(c_sphere.sphere, node->area);

	if (result == CollideType::INSIDE)
		return node;

	else
		return ParentNodeQuery(c_sphere, node->parent_node);
}

bool reality::QuadTreeMgr::LeafNodeQuery(C_CapsuleCollision& c_capsule, SpaceNode* node, vector<SpaceNode*>& node_list)
{
	auto result = CapsuleToAABB(node->area, c_capsule.capsule);

	if (node == root_node_ && result != CollideType::INSIDE)
	{
		return false;
	}

	if (result != CollideType::OUTSIDE)
	{
		if (result == CollideType::INTERSECT)
			c_capsule.enclosed_node_index = node->parent_node->node_num;

		if (node->is_leaf)
		{
			if (result == CollideType::INSIDE)
				c_capsule.enclosed_node_index = node->parent_node->node_num;

			node_list.push_back(node);
			return true;
		}
		if (!node->is_leaf)
		{
			LeafNodeQuery(c_capsule, node->child_node_[0], node_list);
			LeafNodeQuery(c_capsule, node->child_node_[1], node_list);
			LeafNodeQuery(c_capsule, node->child_node_[2], node_list);
			LeafNodeQuery(c_capsule, node->child_node_[3], node_list);
		}
	}
	return true;
}

bool reality::QuadTreeMgr::LeafNodeQuery(C_SphereCollision& c_sphere, SpaceNode* node, vector<SpaceNode*>& node_list)
{
	auto result = SphereToAABB(c_sphere.sphere, node->area);

	if (node == root_node_ && result != CollideType::INSIDE)
	{
		return false;
	}

	if (result != CollideType::OUTSIDE)
	{
		if (result == CollideType::INTERSECT)
			c_sphere.enclosed_node_index = node->parent_node->node_num;

		if (node->is_leaf)
		{
			if (result == CollideType::INSIDE)
				c_sphere.enclosed_node_index = node->parent_node->node_num;

			node_list.push_back(node);
			return true;
		}
		if (!node->is_leaf)
		{
			LeafNodeQuery(c_sphere, node->child_node_[0], node_list);
			LeafNodeQuery(c_sphere, node->child_node_[1], node_list);
			LeafNodeQuery(c_sphere, node->child_node_[2], node_list);
			LeafNodeQuery(c_sphere, node->child_node_[3], node_list);
		}
	}
	return true;
}

bool reality::QuadTreeMgr::IncludingNodeQuery(C_SphereCollision& c_sphere, SpaceNode* node, vector<SpaceNode*>& out_nodes)
{
	CollideType result = SphereToAABB(c_sphere.sphere, node->area);

	if (node == root_node_ && result != CollideType::INSIDE)
		return false;

	if (result != CollideType::OUTSIDE)
	{
		if (node->is_leaf)
		{
			out_nodes.push_back(node);
		}
		else
		{
			IncludingNodeQuery(c_sphere, node->child_node_[0], out_nodes);
			IncludingNodeQuery(c_sphere, node->child_node_[1], out_nodes);
			IncludingNodeQuery(c_sphere, node->child_node_[2], out_nodes);
			IncludingNodeQuery(c_sphere, node->child_node_[3], out_nodes);
		}
	}

	return true;
}

void reality::QuadTreeMgr::RaycastNodeQuery(const RayShape& ray, SpaceNode* node, map<float, RayCallback>& callbacks)
{
	float dist;
	if (node->culling_aabb.Intersects(_XMVECTOR3(ray.start), GetRayDirection(ray), dist))
	{
		if (node->is_leaf)
		{
			for (const auto& tri : node->static_triangles)
			{
				raycast_calculated++;
				auto callback = RayToTriangle(ray, tri);
				if (callback.success)
					callbacks.insert(make_pair(callback.distance, callback));
			}
		}
		else
		{
			RaycastNodeQuery(ray, node->child_node_[0], callbacks);
			RaycastNodeQuery(ray, node->child_node_[1], callbacks);
			RaycastNodeQuery(ray, node->child_node_[2], callbacks);
			RaycastNodeQuery(ray, node->child_node_[3], callbacks);
		}
	}
}

void reality::QuadTreeMgr::SetSpaceHeight(float min_y, float max_y)
{
	for (auto& node : total_nodes_)
	{
		node.second->area.min.y = min_y;
		node.second->area.max.y = max_y;
	}
}

RayCallback reality::QuadTreeMgr::Raycast(const RayShape& ray, entt::entity owner_ent = entt::null)
{
	map<float, RayCallback> callback_list;
	
	RaycastNodeQuery(ray, root_node_, callback_list);

	const auto& c_capsule_view = registry_->view<C_CapsuleCollision>();

	for (const auto& ent : c_capsule_view)
	{
		auto& c_capsule = registry_->get<C_CapsuleCollision>(ent);

		if (c_capsule.hit_enable == false)
			continue;

		if (SCENE_MGR->GetActor<Actor>(ent)->visible == false)
			continue;

		if (ent == owner_ent)
			continue;

		auto callback = RayToCapsule(ray, c_capsule.capsule);
		if (callback.success)
		{
			callback.is_actor = true;
			callback.ent = ent;
			callback_list.insert(make_pair(callback.distance, callback));
		}
	}

	if (callback_list.empty())
		return RayCallback();

	return callback_list.begin()->second;
}

RayCallback reality::QuadTreeMgr::RaycastActorOnly(const RayShape& ray, entt::entity owner_ent = entt::null)
{
	map<float, RayCallback> callback_list;

	const auto& c_capsule_view = registry_->view<C_CapsuleCollision>();

	for (const auto& ent : c_capsule_view)
	{
		auto& c_capsule = registry_->get<C_CapsuleCollision>(ent);

		if (c_capsule.hit_enable == false)
			continue;

		if (SCENE_MGR->GetActor<Actor>(ent)->visible == false)
			continue;

		if (ent == owner_ent)
			continue;

		auto callback = RayToCapsule(ray, c_capsule.capsule);
		if (callback.success)
		{
			callback.is_actor = true;
			callback.ent = ent;
			callback_list.insert(make_pair(callback.distance, callback));
		}
	}

	if (callback_list.empty())
		return RayCallback();

	return callback_list.begin()->second;
}

RayCallback reality::QuadTreeMgr::RaycastCarOnly(const RayShape& ray)
{
	map<float, RayCallback> callback_list;

	for (const auto& tri : car_triagnles_)
	{
		auto callback = RayToTriangle(ray, tri);
		if (callback.success)
		{
			callback.is_actor = false;
			callback_list.insert(make_pair(callback.distance, callback));
		}
	}

	if (callback_list.empty())
		return RayCallback();

	return callback_list.begin()->second;
}

RayCallback reality::QuadTreeMgr::RaycastActorTargeted(const RayShape& ray, entt::entity target_ent)
{
	RayCallback callback;

	auto c_capsule_ptr = registry_->try_get<C_CapsuleCollision>(target_ent);
	if (c_capsule_ptr == nullptr)
		return RayCallback();

	if (c_capsule_ptr->hit_enable == false)
		return RayCallback();

	callback = RayToCapsule(ray, c_capsule_ptr->capsule);
	if (callback.success)
	{
		callback.is_actor = true;
		callback.ent = target_ent;
	}

	return callback;
}

bool reality::QuadTreeMgr::CreatePhysicsCS()
{
	UINT total_size = 0;
	for (auto& leaf_node : leaf_nodes_)
	{
		total_size += leaf_node.second->static_triangles.size();
	}

	triangle_stbuffer.SetElementArraySize(total_size);

	UINT index = 0;
	for (auto& leaf_node : leaf_nodes_)
	{
		for (auto& tri : leaf_node.second->static_triangles)
		{
			triangle_stbuffer.elements[index].including_node = leaf_node.second->node_num;
			triangle_stbuffer.elements[index].normal = tri.normal;
			triangle_stbuffer.elements[index].vertex0 = tri.vertex0;
			triangle_stbuffer.elements[index].vertex1 = tri.vertex1;
			triangle_stbuffer.elements[index].vertex2 = tri.vertex2;

			index++;
		}		
	}

	if (triangle_stbuffer.Create(triangle_stbuffer.elements.data()) == false)
		return false;

	capsule_stbuffer.SetElementArraySize(32);
	if (capsule_stbuffer.Create(capsule_stbuffer.elements.data()) == false)
		return false;

	sphere_stbuffer.SetElementArraySize(32);
	if (sphere_stbuffer.Create(sphere_stbuffer.elements.data()) == false)
		return false;

	result_stbuffer.SetElementArraySize(32);
	if (result_stbuffer.Create(result_stbuffer.elements.data()) == false)
		return false;

	// staging buffer
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC buffer_desc;

	ZeroMemory(&buffer_desc, sizeof(buffer_desc));
	buffer_desc.ByteWidth = sizeof(SbCollisionResult::Data) * 32;
	buffer_desc.Usage = D3D11_USAGE_STAGING;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	hr = DX11APP->GetDevice()->CreateBuffer(&buffer_desc, nullptr, staging_buffer_.GetAddressOf());
	if (FAILED(hr))
		return false;

	ZeroMemory(&empty_pool_, sizeof(empty_pool_));

	return true;
}

bool reality::QuadTreeMgr::InitCollisionMeshes()
{
	HRESULT hr;

	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA subdata;

	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.ByteWidth = sizeof(CbTransform::Data);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	subdata.pSysMem = &capsule_mesh_transform.data;

	hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, capsule_mesh_transform.buffer.GetAddressOf());
	if (FAILED(hr))
		return false;

	return true;
}

void reality::QuadTreeMgr::RenderCollisionMeshes()
{
	if (!view_collisions_)
		return;


	StaticMesh* capsule_mesh = RESOURCE->UseResource<StaticMesh>("CapsuleMesh.stmesh");
	VertexShader* vertex_shader = RESOURCE->UseResource<VertexShader>("CollisionMeshVS.cso");
	Material* material = RESOURCE->UseResource<Material>("Capsule.mat");

	if (capsule_mesh == nullptr || vertex_shader == nullptr || material == nullptr)
		return;

	DX11APP->GetDeviceContext()->VSSetShader(nullptr, nullptr, 0);
	DX11APP->GetDeviceContext()->GSSetShader(nullptr, nullptr, 0);
	DX11APP->GetDeviceContext()->PSSetShader(nullptr, nullptr, 0);

	DX11APP->GetDeviceContext()->IASetInputLayout(vertex_shader->InputLayout());
	DX11APP->GetDeviceContext()->VSSetShader(vertex_shader->Get(), nullptr, 0);
	DX11APP->GetDeviceContext()->RSSetState(DX11APP->GetCommonStates()->Wireframe());
	
	material->Set();

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	const auto& c_capsule_view = registry_->view<C_CapsuleCollision>();
	for (const auto& ent : c_capsule_view)
	{
		auto& c_capsule = registry_->get<C_CapsuleCollision>(ent);
		auto capsule_info = GetTipBaseAB(c_capsule.capsule);

		XMMATRIX a_sphere_world = XMMatrixScaling(c_capsule.capsule.radius, c_capsule.capsule.radius, c_capsule.capsule.radius) * XMMatrixTranslationFromVector(capsule_info[2]);
		XMMATRIX b_sphere_world = XMMatrixScaling(c_capsule.capsule.radius, c_capsule.capsule.radius, c_capsule.capsule.radius) * XMMatrixTranslationFromVector(capsule_info[3]);

		capsule_mesh_transform.data.transform_matrix = XMMatrixTranspose(a_sphere_world);
		DX11APP->GetDeviceContext()->UpdateSubresource(capsule_mesh_transform.buffer.Get(), 0, 0, &capsule_mesh_transform.data, 0, 0);
		DX11APP->GetDeviceContext()->VSSetConstantBuffers(1, 1, capsule_mesh_transform.buffer.GetAddressOf());

		DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, capsule_mesh->meshes[0].vertex_buffer.GetAddressOf(), &stride, &offset);
		DX11APP->GetDeviceContext()->IASetIndexBuffer(capsule_mesh->meshes[0].index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		DX11APP->GetDeviceContext()->DrawIndexed(capsule_mesh->meshes[0].indices.size(), 0, 0);

		capsule_mesh_transform.data.transform_matrix = XMMatrixTranspose(b_sphere_world);
		DX11APP->GetDeviceContext()->UpdateSubresource(capsule_mesh_transform.buffer.Get(), 0, 0, &capsule_mesh_transform.data, 0, 0);
		DX11APP->GetDeviceContext()->VSSetConstantBuffers(1, 1, capsule_mesh_transform.buffer.GetAddressOf());

		DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, capsule_mesh->meshes[1].vertex_buffer.GetAddressOf(), &stride, &offset);
		DX11APP->GetDeviceContext()->IASetIndexBuffer(capsule_mesh->meshes[0].index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		DX11APP->GetDeviceContext()->DrawIndexed(capsule_mesh->meshes[1].indices.size(), 0, 0);
	}

	capsule_mesh = nullptr;
	vertex_shader = nullptr;
	material = nullptr;

	deviding_level_->RenderCollisionMesh();

	DX11APP->GetDeviceContext()->RSSetState(DX11APP->GetCommonStates()->CullNone());
}

const map<UINT, SpaceNode*>& reality::QuadTreeMgr::GetLeafNodes()
{
	return leaf_nodes_;
}

void reality::QuadTreeMgr::RunPhysicsCS(string cs_id)
{
	auto compute_shader = RESOURCE->UseResource<ComputeShader>(cs_id);
	if (compute_shader == nullptr)
		return;

	DX11APP->GetDeviceContext()->CSSetShader(compute_shader->Get(), nullptr, 0);


	DX11APP->GetDeviceContext()->CSSetShaderResources(0, 1, triangle_stbuffer.srv.GetAddressOf());	
	DX11APP->GetDeviceContext()->CSSetShaderResources(1, 1, capsule_stbuffer.srv.GetAddressOf());
	DX11APP->GetDeviceContext()->CSSetShaderResources(2, 1, sphere_stbuffer.srv.GetAddressOf());
	DX11APP->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, result_stbuffer.uav.GetAddressOf(), 0);
	DX11APP->GetDeviceContext()->Dispatch(1, 1, 1);

	DX11APP->GetDeviceContext()->CopyResource(staging_buffer_.Get(), result_stbuffer.buffer.Get());

	D3D11_MAPPED_SUBRESOURCE mapped_resource = { 0, };
	
	HRESULT hr = DX11APP->GetDeviceContext()->Map(staging_buffer_.Get(), 0, D3D11_MAP_READ, 0, &mapped_resource);
	if (FAILED(hr))
		return;

	SbCollisionResult::Data* sum = reinterpret_cast<SbCollisionResult::Data*>(mapped_resource.pData);
	memcpy(collision_result_pool_.data(), sum, 32 * sizeof(SbCollisionResult::Data));

	DX11APP->GetDeviceContext()->Unmap(staging_buffer_.Get(), 0);
	DX11APP->GetDeviceContext()->UpdateSubresource(result_stbuffer.buffer.Get(), 0, 0, empty_pool_.data(), 0, 0);
}

void reality::QuadTreeMgr::MovementByPhysicsCS()
{
	for (UINT i = 0; i < 32; ++i)
	{
		auto& result = collision_result_pool_[i];
		auto character = SCENE_MGR->GetActor<Character>((entt::entity)result.capsule_result.entity);
		if (character == nullptr)
			continue;
		character->floor_position = result.capsule_result.floor_position;
		switch (result.capsule_result.collide_type)
		{
		case 0: 
			character->movement_state_ = MovementState::FALL;
			character->GetMovementComponent()->gravity_pulse = 300.0f;
			break;
		case 1: 
			character->movement_state_ = MovementState::WALK;
			character->GetMovementComponent()->gravity_pulse = 0.0f;
			character->GetMovementComponent()->velocity.m128_f32[1] = 0.0f;
			break;
		case 2: 
			character->movement_state_ = MovementState::FALL;
			character->GetMovementComponent()->gravity_pulse = 300.0f;
			break;
		case 3:
			character->movement_state_ = MovementState::WALK;
			character->GetMovementComponent()->gravity_pulse = 0.0f;
			character->GetMovementComponent()->velocity.m128_f32[1] = 0.0f;
			break;
		}

		for (int j = 0; j < 4; ++j)
		{
			character->blocking_planes_[j] = result.capsule_result.wall_planes[j];
		}

		character = nullptr;
	}
}

void reality::QuadTreeMgr::SphereUpdateByPhysicsCS()
{
	for (UINT i = 0; i < 32; ++i)
	{
		auto& result = collision_result_pool_[i].sphere_result;
		auto sphere_comp = registry_->try_get<C_SphereCollision>((entt::entity)result.entity);
		if (sphere_comp == nullptr)
			continue;

		sphere_comp->is_collide = 0;
		sphere_comp->tri_normal = { 0.0f, 0.0f, 0.0f };

		if (result.is_collide)
		{
			sphere_comp->is_collide = result.is_collide;
			sphere_comp->tri_normal = result.tri_normal;
		}
	}
}

void reality::QuadTreeMgr::CapsuleImpulse(entt::entity ent, C_CapsuleCollision& c_capsule)
{
	XMVECTOR impulse_vec = XMVector3Normalize(c_capsule.impulse_vector);
	float strength = c_capsule.impulse_strength;

	if (strength >= 0.001f)
	{
		auto character = SCENE_MGR->GetActor<Character>(ent);
		if (character == nullptr)
			return;

		impulse_vec = impulse_vec * strength * TM_DELTATIME;
		impulse_vec.m128_f32[1] = max(0, impulse_vec.m128_f32[1]);

		XMVECTOR actor_pos = character->GetCurPosition();

		for (int i = 0; i < 4; ++i)
		{
			auto plane = character->blocking_planes_[i];

			if (Vector3Length(_XMVECTOR4(plane)) <= 0.1f)
				continue;

			XMVECTOR plane_normal = XMVectorSet(plane.x, plane.y, plane.z, 0);

			if (XMVectorGetX(XMVector3Dot(plane_normal, impulse_vec)) < 0.0f)
				impulse_vec = VectorProjectPlane(impulse_vec, plane_normal);

		}
		character->ApplyMovement(actor_pos + impulse_vec);
		c_capsule.impulse_strength -= Vector3Length(impulse_vec) * 5;
	}
}

void reality::QuadTreeMgr::ExportQuadTreeData(string filename)
{
	FileTransfer outdata_file(filename, WRITE);

	// physics tree data
	UINT physics_node_count = total_nodes_.size();
	outdata_file.WriteBinary<UINT>(&physics_node_count, 1);

	for (auto& node : total_nodes_)
	{
		// index data
		UINT null_node = 99999;

		outdata_file.WriteBinary<UINT>(&node.second->node_num, 1);
		outdata_file.WriteBinary<bool>(&node.second->is_leaf, 1);

		if (node.second->parent_node)			
		{
			outdata_file.WriteBinary<UINT>(&node.second->parent_node->node_num, 1);
		}
		else
		{
			outdata_file.WriteBinary<UINT>(&null_node, 1);
		}

		if (node.second->child_node_[0])
		{
			outdata_file.WriteBinary<UINT>(&node.second->child_node_[0]->node_num, 1);
			outdata_file.WriteBinary<UINT>(&node.second->child_node_[1]->node_num, 1);
			outdata_file.WriteBinary<UINT>(&node.second->child_node_[2]->node_num, 1);
			outdata_file.WriteBinary<UINT>(&node.second->child_node_[3]->node_num, 1);
		}
		else
		{
			outdata_file.WriteBinary<UINT>(&null_node, 1);
			outdata_file.WriteBinary<UINT>(&null_node, 1);
			outdata_file.WriteBinary<UINT>(&null_node, 1);
			outdata_file.WriteBinary<UINT>(&null_node, 1);
		}

		// aabb data
		outdata_file.WriteBinary<XMFLOAT3>(&node.second->area.min, 1);
		outdata_file.WriteBinary<XMFLOAT3>(&node.second->area.max, 1);
		outdata_file.WriteBinary<XMFLOAT3>(&node.second->area.center, 1);

		// collision data
		UINT triangles_count = node.second->static_triangles.size();
		outdata_file.WriteBinary<UINT>(&triangles_count, 1);

		for (auto& tri : node.second->static_triangles)
		{
			outdata_file.WriteBinary<UINT>(&tri.index, 1);
			outdata_file.WriteBinary<XMFLOAT3>(&tri.normal, 1);
			outdata_file.WriteBinary<XMFLOAT3>(&tri.vertex0, 1);
			outdata_file.WriteBinary<XMFLOAT3>(&tri.vertex1, 1);
			outdata_file.WriteBinary<XMFLOAT3>(&tri.vertex2, 1);			
		}
	}
}

