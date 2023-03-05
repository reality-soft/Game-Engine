#include "stdafx.h"
#include "StaticObject.h"

void reality::StaticObject::OnInit(entt::registry& registry, reality::C_Transform transform, AABBShape collision_box, string static_mesh_id)
{
	entity_id_ = registry.create();
	
	// Add components to registry
	registry.emplace<reality::C_Transform>(entity_id_, transform);

	reality::C_BoundingBox bounding_box;
	bounding_box.local = XMMatrixIdentity();
	bounding_box.aabb = collision_box;
	registry.emplace<reality::C_BoundingBox>(entity_id_, bounding_box);

	C_StaticMesh static_mesh;
	static_mesh.vertex_shader_id = "StaticMeshVS.cso";
	static_mesh.static_mesh_id = static_mesh_id;

	static_mesh.local = XMMatrixIdentity();
	registry.emplace<C_StaticMesh>(entity_id_, static_mesh);

	// form transform tree
	transform_tree_.root_node = make_shared<TransformTreeNode>(TYPE_ID(reality::C_Transform));
	transform_tree_.AddNodeToNode(TYPE_ID(reality::C_Transform), TYPE_ID(reality::C_BoundingBox));
	transform_tree_.AddNodeToNode(TYPE_ID(reality::C_BoundingBox), TYPE_ID(reality::C_StaticMesh));

	// update transforms of components that belongs to transform tree
	transform_tree_.root_node->OnUpdate(registry, entity_id_);

	// get updated bounding box
	bounding_box = registry.get<reality::C_BoundingBox>(entity_id_);

	// add this object to quad tree node
	//this->node_num_ = QUADTREE->UpdateNodeObjectBelongs(0, bounding_box.aabb, entity_id_);
}

void reality::StaticObject::OnUpdate(entt::registry& registry)
{
	reality::C_BoundingBox bounding_box = registry.get<reality::C_BoundingBox>(entity_id_);
	//this->node_num_ = QUADTREE->UpdateNodeObjectBelongs(this->node_num_, bounding_box.aabb, entity_id_);
	transform_tree_.root_node->OnUpdate(registry, entity_id_, transform);
}

void reality::StaticObject::SetTransformTRS(const XMVECTOR& t, const XMVECTOR& r, const XMVECTOR& s)
{
	transform = XMMatrixIdentity();

	XMMATRIX tm = XMMatrixTranslationFromVector(t);
	XMMATRIX rm = XMMatrixRotationRollPitchYawFromVector(r);
	XMMATRIX sm = XMMatrixScalingFromVector(s);

	transform *= tm * rm * sm;
}