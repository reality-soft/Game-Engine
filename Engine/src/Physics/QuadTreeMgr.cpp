#include "stdafx.h"
#include "FileTransfer.h"
#include "QuadTreeMgr.h"
#include "TimeMgr.h"
#include "SceneMgr.h"
#include "Character.h"

using namespace reality;

reality::PhysicsNode::PhysicsNode(UINT num, UINT depth)
{
	node_num = num;
	node_depth = depth;
}

reality::PhysicsNode::~PhysicsNode()
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

void reality::PhysicsNode::SetNode(float min_x, float min_z, float max_x, float max_z)
{
	XMFLOAT3 min = XMFLOAT3(min_x, MIN_HEIGHT, min_z);
	XMFLOAT3 max = XMFLOAT3(max_x, MAX_HEIGHT, max_z);
	area = AABBShape(min, max);
}

reality::MeshNode::MeshNode(UINT num, UINT depth)
{
	node_num = num;
	node_depth = depth;
}

reality::MeshNode::~MeshNode()
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

void reality::MeshNode::SetNode(float min_x, float min_z, float max_x, float max_z)
{
	XMFLOAT3 min = XMFLOAT3(min_x, MIN_HEIGHT, min_z);
	XMFLOAT3 max = XMFLOAT3(max_x, MAX_HEIGHT, max_z);
	area = AABBShape(min, max);
}

void reality::QuadTreeMgr::Init(StaticMeshLevel* level_to_devide, entt::registry& reg)
{
	deviding_level_ = level_to_devide;
	registry_ = &reg;

	// regist dynamic capsule
	const auto& capsule_view = registry_->view<C_CapsuleCollision>();
	for (auto& ent : capsule_view)
	{
		auto capsule_collision = registry_->try_get<C_CapsuleCollision>(ent);
		dynamic_capsule_list.insert(make_pair(ent, capsule_collision));
	}
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
	root_physics_node_ = BuildPhysicsTree(0, min_x, min_z, max_x, max_z);

	// set blocking field
	for (auto& guide_line : deviding_level_->GetGuideLines(GuideLine::GuideType::eBlocking))
	{
		if (guide_line.guide_type_ == GuideLine::GuideType::eBlocking)
		{
			for (UINT i = 1; i < guide_line.line_nodes.size(); ++i)
			{
				RayShape blocking_line;
				blocking_line.start = _XMFLOAT3(guide_line.line_nodes.at(i - 1));
				blocking_line.end = _XMFLOAT3(guide_line.line_nodes.at(i));

				blocking_line.start.y = 0.0f;
				blocking_line.end.y = 0.0f;

				blocking_lines.push_back(blocking_line);
			}
		}
	}

	// generate level mesh to triangles
	min_x = -10000;
	min_z = -10000;
	max_x = 10000;
	max_z = 10000;
	node_count = 0;
	root_mesh_node_ = BuildMeshTree(0, min_x, min_z, max_x, max_z);

	SetMeshes(root_mesh_node_);
}

void reality::QuadTreeMgr::ImportQuadTreeData(string filename)
{
	FileTransfer read_file(filename, READ);

	UINT null_node_index = 99999;

	// physics tree data
	UINT physics_node_count;
	read_file.ReadBinary<UINT>(physics_node_count);

	for (UINT n = 0; n < physics_node_count; ++n)
	{
		// index data
		PhysicsNode* new_node = new PhysicsNode(n, 0);
		total_physics_nodes_.insert(make_pair(n, new_node));

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

	// physics tree data
	UINT mesh_node_count;
	read_file.ReadBinary<UINT>(mesh_node_count);

	for (UINT n = 0; n < mesh_node_count; ++n)
	{
		// index data
		MeshNode* new_node = new MeshNode(n, 0);
		total_mesh_nodes_.insert(make_pair(n, new_node));

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

		// mesh data
		UINT mesh_size = 0;
		read_file.ReadBinary<UINT>(mesh_size);
		new_node->separated_level_mesh_.meshes.resize(mesh_size);

		for (UINT m = 0; m < mesh_size; ++m)
		{
			auto& mesh = new_node->separated_level_mesh_.meshes[m];

			UINT vertex_size;

			read_file.ReadBinary<UINT>(vertex_size);
			read_file.ReadBinary<string>(mesh.mesh_name);

			mesh.vertices.resize(vertex_size);
			read_file.ReadBinary<Vertex>(mesh.vertices);
		}
	}

	InitImported();
}

void reality::QuadTreeMgr::InitImported()
{
	for (auto& physics_node : total_physics_nodes_)
	{
		if (physics_node.second->parent_node_index != 99999)
		{
			physics_node.second->parent_node = total_physics_nodes_.find(physics_node.second->parent_node_index)->second;
		}
		if (physics_node.second->child_node_index[0] != 99999)
		{
			physics_node.second->child_node_[0] = total_physics_nodes_.find(physics_node.second->child_node_index[0])->second;
			physics_node.second->child_node_[1] = total_physics_nodes_.find(physics_node.second->child_node_index[1])->second;
			physics_node.second->child_node_[2] = total_physics_nodes_.find(physics_node.second->child_node_index[2])->second;
			physics_node.second->child_node_[3] = total_physics_nodes_.find(physics_node.second->child_node_index[3])->second;
		}

		if (physics_node.second->is_leaf)
			leaf_physics_nodes_.insert(physics_node);
	}

	for (auto& mesh_node : total_mesh_nodes_)
	{
		if (mesh_node.second->parent_node_index != 99999)
		{
			mesh_node.second->parent_node = total_mesh_nodes_.find(mesh_node.second->parent_node_index)->second;
		}
		if (mesh_node.second->child_node_index[0] != 99999)
		{
			mesh_node.second->child_node_[0] = total_mesh_nodes_.find(mesh_node.second->child_node_index[0])->second;
			mesh_node.second->child_node_[1] = total_mesh_nodes_.find(mesh_node.second->child_node_index[1])->second;
			mesh_node.second->child_node_[2] = total_mesh_nodes_.find(mesh_node.second->child_node_index[2])->second;
			mesh_node.second->child_node_[3] = total_mesh_nodes_.find(mesh_node.second->child_node_index[3])->second;
		}
		for (auto& mesh : mesh_node.second->separated_level_mesh_.meshes)
		{
			if (mesh.vertices.size() > 0)
				bool buffer_created = CreateVertexBuffer(mesh);
		}

		if (mesh_node.second->is_leaf)
			leaf_mesh_nodes_.insert(mesh_node);
	}
}

PhysicsNode* reality::QuadTreeMgr::BuildPhysicsTree(UINT depth, float min_x, float min_z, float max_x, float max_z)
{
	PhysicsNode* new_node = new PhysicsNode(node_count++, depth);

	new_node->SetNode(min_x, min_z, max_x, max_z);
	total_physics_nodes_.insert(make_pair(new_node->node_num, new_node));

	if (new_node->node_depth == max_depth)
	{
		new_node->is_leaf = true;
		SetStaticTriangles(new_node);
		if (new_node->static_triangles.size() > 0)
			leaf_physics_nodes_.insert(make_pair(new_node->node_num, new_node));
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

void reality::QuadTreeMgr::SetStaticTriangles(PhysicsNode* node)
{
	for (auto& tri : deviding_level_->level_triangles)
	{
		CollideType result = AABBToTriagnle(node->area, tri);
		if (result != CollideType::OUTSIDE)
		{
			node->static_triangles.push_back(tri);
		}
	}
}

MeshNode* reality::QuadTreeMgr::BuildMeshTree(UINT depth, float min_x, float min_z, float max_x, float max_z)
{
	MeshNode* new_node = new MeshNode(node_count++, depth);

	new_node->SetNode(min_x, min_z, max_x, max_z);
	total_mesh_nodes_.insert(make_pair(new_node->node_num, new_node));

	if (new_node->node_depth == max_depth)
	{
		new_node->is_leaf = true;
		leaf_mesh_nodes_.insert(make_pair(new_node->node_num, new_node));
	}

	if (depth < max_depth)
	{
		float row_mid = (min_x + max_x) / 2;
		float col_mid = (min_z + max_z) / 2;

		new_node->child_node_[0] = BuildMeshTree(depth + 1, min_x, min_z, row_mid, col_mid);
		new_node->child_node_[0]->parent_node = new_node;

		new_node->child_node_[1] = BuildMeshTree(depth + 1, row_mid, min_z, max_x, col_mid);
		new_node->child_node_[1]->parent_node = new_node;

		new_node->child_node_[2] = BuildMeshTree(depth + 1, min_x, col_mid, row_mid, max_z);
		new_node->child_node_[2]->parent_node = new_node;

		new_node->child_node_[3] = BuildMeshTree(depth + 1, row_mid, col_mid, max_x, max_z);
		new_node->child_node_[3]->parent_node = new_node;
	}

	return new_node;
}

void reality::QuadTreeMgr::SetMeshes(MeshNode* node)
{
	if (node == root_mesh_node_)
		node->separated_level_mesh_ = *deviding_level_->GetLevelMesh();

	else
	{
		for (auto& mesh : node->parent_node->separated_level_mesh_.meshes)
		{
			SingleMesh<Vertex> new_single_mesh;

			UINT num_triangle = mesh.vertices.size() / 3;
			UINT index = 0;
			for (UINT t = 0; t < num_triangle; t++)
			{
				TriangleShape mesh_tri = TriangleShape(
					mesh.vertices[index + 0].p,
					mesh.vertices[index + 1].p,
					mesh.vertices[index + 2].p
				);

				if (AABBToTriagnle(node->area, mesh_tri) != CollideType::OUTSIDE)
				{
					new_single_mesh.vertices.push_back(mesh.vertices[index + 0]);
					new_single_mesh.vertices.push_back(mesh.vertices[index + 1]);
					new_single_mesh.vertices.push_back(mesh.vertices[index + 2]);
				}

				index += 3;
			}

			if (new_single_mesh.vertices.empty())
				continue;

			if (CreateVertexBuffer(new_single_mesh))
				node->separated_level_mesh_.meshes.push_back(new_single_mesh); 
		}
	}

	if (node->is_leaf)
		return;

	SetMeshes(node->child_node_[0]);
	SetMeshes(node->child_node_[1]);
	SetMeshes(node->child_node_[2]);
	SetMeshes(node->child_node_[3]);
}

void reality::QuadTreeMgr::Frame(CameraSystem* applied_camera)
{
	camera_frustum_ = Frustum(applied_camera->GetViewProj());
	UpdateCapsules();
}

void reality::QuadTreeMgr::Release()
{
	if (root_physics_node_)
	{
		delete root_physics_node_;
		root_physics_node_ = nullptr;
	}
	total_physics_nodes_.clear();
	leaf_physics_nodes_.clear();
}

void reality::QuadTreeMgr::UpdatePhysics(string cs_id)
{
	UINT capsule_index = 0;
	for (auto& dynamic_capsule : dynamic_capsule_list)
	{
		PhysicsNode* query_start = total_physics_nodes_.find(dynamic_capsule.second->enclosed_node_index)->second;
		query_start = ParentNodeQuery(dynamic_capsule.second, query_start);

		vector<PhysicsNode*> queried_nodes;
		LeafNodeQuery(dynamic_capsule.second, query_start, queried_nodes);

		if (queried_nodes.empty())
			continue;

		auto capsule_info = dynamic_capsule.second->capsule.GetTipBaseAB();
		capsule_stbuffer.elements[capsule_index].point_a = _XMFLOAT3(capsule_info[2]);
		capsule_stbuffer.elements[capsule_index].point_b = _XMFLOAT3(capsule_info[3]);
		capsule_stbuffer.elements[capsule_index].radius = dynamic_capsule.second->capsule.radius;
		capsule_stbuffer.elements[capsule_index].entity = (int)dynamic_capsule.first;

		for (int i = 0; i < 4; ++i)
		{
			if (i >= queried_nodes.size())
				capsule_stbuffer.elements[capsule_index].node_numbers[i] = 0;

			else
				capsule_stbuffer.elements[capsule_index].node_numbers[i] = queried_nodes[i]->node_num;
		}


		capsule_index++;
	}

	DX11APP->GetDeviceContext()->UpdateSubresource(capsule_stbuffer.buffer.Get(), 0, 0, capsule_stbuffer.elements.data(), 0, 0);
	RunPhysicsCS(cs_id);
	MovementByPhysicsCS();
}

void reality::QuadTreeMgr::NodeCasting(const RayShape& ray, PhysicsNode* node)
{
	if (FrustumToAABB(camera_frustum_, node->area) == CollideType::OUTSIDE)
		return;
	
	if (RayToAABB(ray, node->area))
	{
		if (node->is_leaf && node->static_triangles.size() > 0)
		{
			float dist = Distance(_XMVECTOR3(node->area.center), _XMVECTOR3(ray.start));
			casted_physics_nodes_.insert(make_pair(dist, node));
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

PhysicsNode* reality::QuadTreeMgr::ParentNodeQuery(C_CapsuleCollision* c_capsule, PhysicsNode* node)
{
	if (node == nullptr)
		return root_physics_node_;

	auto capsule_shape = c_capsule->capsule;
	auto result = CapsuleToAABB(node->area, capsule_shape);

	if (result == CollideType::INSIDE)
		return node;

	else
		return ParentNodeQuery(c_capsule, node->parent_node);
}

bool reality::QuadTreeMgr::LeafNodeQuery(C_CapsuleCollision* c_capsule, PhysicsNode* node, vector<PhysicsNode*>& node_list)
{
	auto capsule_shape = c_capsule->capsule;
	auto result = CapsuleToAABB(node->area, capsule_shape);

	if (node == root_physics_node_ && result != CollideType::INSIDE)
	{
		return false;
	}

	if (result != CollideType::OUTSIDE)
	{
		if (result == CollideType::INTERSECT)
			c_capsule->enclosed_node_index = node->parent_node->node_num;

		if (node->is_leaf)
		{
			if (result == CollideType::INSIDE)
				c_capsule->enclosed_node_index = node->parent_node->node_num;

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

void reality::QuadTreeMgr::UpdateCapsules()
{
	vector<entt::entity> destroied_actors;

	for (auto& c_capsule : dynamic_capsule_list)
	{
		if (SCENE_MGR->GetActor<Actor>(c_capsule.first) == nullptr)
		{
			destroied_actors.push_back(c_capsule.first);
			c_capsule.second = nullptr;
		}
	}

	for (const auto& ent : destroied_actors)
	{
		dynamic_capsule_list.erase(ent);
	}
}

//pair<RayCallback, entt::entity> reality::QuadTreeMgr::RaycastAdjustActor(RayShape& ray)
//{
//	map<float, RayCallback> callback_list;
//
//	entt::entity selected_entity = entt::null;
//
//	auto level_callback = RaycastAdjustLevel(ray, 15000);
//	for (auto& capsule : dynamic_capsule_list)
//	{
//		if (capsule.first == SCENE_MGR->GetPlayer<Character>(0)->GetEntityId())
//			continue;
//
//		const auto& capsule_callback = RayToCapsule(ray, capsule.second->capsule);
//		if (capsule_callback.success)
//		{
//			callback_list.insert(make_pair(capsule_callback.distance, capsule_callback));
//			selected_entity = capsule.first;
//		}
//	}
//
//	if (callback_list.begin() == callback_list.end())
//		return make_pair(RayCallback(), entt::null);
//
//	if (level_callback.success && level_callback.distance < callback_list.begin()->first)
//		return make_pair(RayCallback(), entt::null);
//
//	return make_pair(callback_list.begin()->second, selected_entity);
//}

void reality::QuadTreeMgr::RegistDynamicCapsule(entt::entity ent)
{
	auto capsule_collision = registry_->try_get<C_CapsuleCollision>(ent);
	if (capsule_collision != nullptr)
		dynamic_capsule_list.insert(make_pair(ent, capsule_collision));
}

bool reality::QuadTreeMgr::CreatePhysicsCS()
{
	UINT total_size = 0;
	for (auto& leaf_node : leaf_physics_nodes_)
	{
		total_size += leaf_node.second->static_triangles.size();
	}

	triangle_stbuffer.SetElementArraySize(total_size);

	UINT index = 0;
	for (auto& leaf_node : leaf_physics_nodes_)
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

	capsule_stbuffer.SetElementArraySize(64);
	if (capsule_stbuffer.Create(capsule_stbuffer.elements.data()) == false)
		return false;

	result_stbuffer.SetElementArraySize(64);
	if (result_stbuffer.Create(result_stbuffer.elements.data()) == false)
		return false;

	// staging buffer
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC buffer_desc;

	ZeroMemory(&buffer_desc, sizeof(buffer_desc));
	buffer_desc.ByteWidth = sizeof(SbCollisionResult::Data) * 64;
	buffer_desc.Usage = D3D11_USAGE_STAGING;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	hr = DX11APP->GetDevice()->CreateBuffer(&buffer_desc, nullptr, staging_buffer_.GetAddressOf());
	if (FAILED(hr))
		return false;

	return true;
}

void reality::QuadTreeMgr::RunPhysicsCS(string cs_id)
{
	auto compute_shader = RESOURCE->UseResource<ComputeShader>(cs_id);
	if (compute_shader == nullptr)
		return;

	DX11APP->GetDeviceContext()->CSSetShader(compute_shader->Get(), nullptr, 0);


	DX11APP->GetDeviceContext()->CSSetShaderResources(0, 1, triangle_stbuffer.srv.GetAddressOf());	
	DX11APP->GetDeviceContext()->CSSetShaderResources(1, 1, capsule_stbuffer.srv.GetAddressOf());
	DX11APP->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, result_stbuffer.uav.GetAddressOf(), 0);
	DX11APP->GetDeviceContext()->Dispatch(1, 1, 1);

	DX11APP->GetDeviceContext()->CopyResource(staging_buffer_.Get(), result_stbuffer.buffer.Get());

	D3D11_MAPPED_SUBRESOURCE mapped_resource = { 0, };
	HRESULT hr = DX11APP->GetDeviceContext()->Map(staging_buffer_.Get(), 0, D3D11_MAP_READ, 0, &mapped_resource);
	if (FAILED(hr))
		return;

	SbCollisionResult::Data* sum = reinterpret_cast<SbCollisionResult::Data*>(mapped_resource.pData);
	memcpy(collision_result_pool_.data(), sum, 64 * sizeof(SbCollisionResult::Data));
	UINT size = 64 * sizeof(SbCollisionResult::Data);
	mapped_resource.RowPitch;
	DX11APP->GetDeviceContext()->Unmap(staging_buffer_.Get(), 0);

}

void reality::QuadTreeMgr::MovementByPhysicsCS()
{
	for (UINT i = 0; i < 64; ++i)
	{
		auto& result = collision_result_pool_[i];
		auto character = SCENE_MGR->GetActor<Character>((entt::entity)result.entity);
		if (character == nullptr)
			continue;

		character->floor_position = result.floor_position;

		if (result.collide_type != 0)
		{
			if (result.collide_type == 1)
			{
				character->GetMovementComponent()->gravity = 0.0f;
				character->movement_state_ = MovementState::STAND_ON_FLOOR;
			}
			if (result.collide_type == 2)
			{
				
			}	
		}
		else
		{
			character->GravityFall(9.81f);
			character->movement_state_ = MovementState::GRAVITY_FALL;
		}

		//if (result.is_wall_collide)
		//{
		//	for (int j = 0; j < 4; ++j)
		//	{
		//		character->blocking_walls_.push_back(result.blocking_rays[j]);
		//	}
		//}
	}
}

void reality::QuadTreeMgr::ExportQuadTreeData(string filename)
{
	FileTransfer outdata_file(filename, WRITE);

	// physics tree data
	UINT physics_node_count = total_physics_nodes_.size();
	outdata_file.WriteBinary<UINT>(&physics_node_count, 1);

	for (auto& node : total_physics_nodes_)
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

	// mesh tree data
	UINT mesh_node_count = total_mesh_nodes_.size();
	outdata_file.WriteBinary<UINT>(&mesh_node_count, 1);

	for (auto& node : total_mesh_nodes_)
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

		// mesh data
		UINT mesh_size = node.second->separated_level_mesh_.meshes.size();
		outdata_file.WriteBinary<UINT>(&mesh_size, 1);

		for (auto& mesh : node.second->separated_level_mesh_.meshes)
		{
			UINT vertex_size = mesh.vertices.size();
			outdata_file.WriteBinary<UINT>(&vertex_size, 1);
			outdata_file.WriteBinary<string>(&mesh.mesh_name, 1);
			outdata_file.WriteBinary<Vertex>(mesh.vertices.data(), vertex_size);
		}
	}
}

