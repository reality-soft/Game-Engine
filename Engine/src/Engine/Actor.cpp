#include "Actor.h"


KGCA41B::Actor::Actor(entt::registry& registry)
{
	OnInit(registry);
}

void KGCA41B::Actor::OnInit(entt::registry& registry)
{
	entity_id_ = registry.create();
	
	entt::type_hash<Transform> type_hash_transform;
	Transform transform;
	transform.local = XMMATRIX(
		1, 1, 1, 1,
		1, 1, 1, 1,
		1, 1, 1, 1,
		1, 1, 1, 1
	);
	registry.emplace<Transform>(entity_id_, transform);

	entt::type_hash<StaticMesh> type_hash_static_mesh;
	StaticMesh static_mesh;
	static_mesh.local = XMMATRIX(
		2, 2, 2, 2,
		2, 2, 2, 2,
		2, 2, 2, 2,
		2, 2, 2, 2
	);
	registry.emplace<Transform>(entity_id_, static_mesh);

	entt::type_hash<SkeletalMesh> type_hash_skeletal_mesh;
	StaticMesh skeletal_mesh;
	skeletal_mesh.local = XMMATRIX(
		2, 2, 2, 2,
		2, 2, 2, 2,
		2, 2, 2, 2,
		2, 2, 2, 2
	);
	registry.emplace<Transform>(entity_id_, skeletal_mesh);

	transform_tree_.root_node = make_shared<TransformTreeNode>(type_hash_transform.value());
	transform_tree_.root_node->children.push_back(make_shared<TransformTreeNode>(type_hash_static_mesh.value()));
	transform_tree_.root_node->children.push_back(make_shared<TransformTreeNode>(type_hash_skeletal_mesh.value()));

	transform_tree_.root_node->OnUpdate(registry, entity_id_);
}