#include "Actor.h"
#include "SpacePartition/SpacePartition.h"

void KGCA41B::Actor::OnInit(entt::registry& registry, AABB<3> collision_box)
{
	collision_box_ = collision_box;

	entity_id_ = registry.create();
	this->node_num_ = SpacePartition::GetInst()->UpdateNodeObjectBelongs(0, collision_box_, entity_id_);
	
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
	registry.emplace<StaticMesh>(entity_id_, static_mesh);

	entt::type_hash<SkeletalMesh> type_hash_skeletal_mesh;
	SkeletalMesh skeletal_mesh;
	skeletal_mesh.local = XMMATRIX(
		3, 3, 3, 3,
		3, 3, 3, 3,
		3, 3, 3, 3,
		3, 3, 3, 3
	);
	registry.emplace<SkeletalMesh>(entity_id_, skeletal_mesh);

	transform_tree_.root_node = make_shared<TransformTreeNode>(type_hash_transform.value());
	transform_tree_.root_node->children.push_back(make_shared<TransformTreeNode>(type_hash_static_mesh.value()));
	transform_tree_.root_node->children.push_back(make_shared<TransformTreeNode>(type_hash_skeletal_mesh.value()));

	transform_tree_.root_node->OnUpdate(registry, entity_id_);
}

void KGCA41B::Actor::OnUpdate(entt::registry& registry)
{
	this->node_num_ = SpacePartition::GetInst()->UpdateNodeObjectBelongs(0, collision_box_, entity_id_);
	vector<int> node_to_search = SpacePartition::GetInst()->FindCollisionSearchNode(0, collision_box_);
	transform_tree_.root_node->OnUpdate(registry, entity_id_);

	unordered_set<entt::entity> object_to_collision_check;
	for (int node : node_to_search) {
		unordered_set<entt::entity> new_objects = SpacePartition::GetInst()->GetObjectListInNode(node_num_);
		object_to_collision_check.insert(new_objects.begin(), new_objects.end());
	}
}
