#include "stdafx.h"
#include "StaticObject.h"

void KGCA41B::StaticObject::OnInit(entt::registry& registry, KGCA41B::Transform transform, AABBShape collision_box, string static_mesh_id)
{
	entity_id_ = registry.create();
	
	// Add components to registry
	registry.emplace<KGCA41B::Transform>(entity_id_, transform);

	KGCA41B::BoundingBox bounding_box;
	bounding_box.local = XMMatrixIdentity();
	bounding_box.aabb = collision_box;
	registry.emplace<KGCA41B::BoundingBox>(entity_id_, bounding_box);

	StaticMesh static_mesh;
	static_mesh.mesh_id = static_mesh_id;
	static_mesh.local = XMMatrixIdentity();
	registry.emplace<StaticMesh>(entity_id_, static_mesh);

	// form transform tree
	transform_tree_.root_node = make_shared<TransformTreeNode>(TYPE_ID(KGCA41B::Transform));
	transform_tree_.AddNodeToNode(TYPE_ID(KGCA41B::Transform), TYPE_ID(KGCA41B::BoundingBox));
	transform_tree_.root_node->children.push_back(make_shared<TransformTreeNode>(TYPE_ID(StaticMesh)));
	transform_tree_.root_node->children.push_back(make_shared<TransformTreeNode>(TYPE_ID(SkeletalMesh)));

	// update transforms of components that belongs to transform tree
	transform_tree_.root_node->OnUpdate(registry, entity_id_);

	// get updated bounding box
	bounding_box = registry.get<KGCA41B::BoundingBox>(entity_id_);

	// add this object to quad tree node
	this->node_num_ = QUADTREE->UpdateNodeObjectBelongs(0, bounding_box.aabb, entity_id_);
}

void KGCA41B::StaticObject::OnUpdate(entt::registry& registry)
{
	KGCA41B::BoundingBox bounding_box = registry.get<KGCA41B::BoundingBox>(entity_id_);
	this->node_num_ = QUADTREE->UpdateNodeObjectBelongs(this->node_num_, bounding_box.aabb, entity_id_);
	transform_tree_.root_node->OnUpdate(registry, entity_id_);
}
