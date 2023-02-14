#include "stdafx.h"
#include "Actor.h"

void KGCA41B::Actor::OnInit(entt::registry& registry, AABB<3> collision_box)
{
	collision_box_ = collision_box;

	entity_id_ = registry.create();
	this->node_num_ = SpacePartition::GetInst()->UpdateNodeObjectBelongs(0, collision_box_, entity_id_);
	
	Transform transform;
	transform.local = XMMATRIX(
		1, 1, 1, 1,
		1, 1, 1, 1,
		1, 1, 1, 1,
		1, 1, 1, 1
	);
	registry.emplace<Transform>(entity_id_, transform);

	StaticMesh static_mesh;
	static_mesh.local = XMMATRIX(
		2, 2, 2, 2,
		2, 2, 2, 2,
		2, 2, 2, 2,
		2, 2, 2, 2
	);
	registry.emplace<StaticMesh>(entity_id_, static_mesh);

	SkeletalMesh skeletal_mesh;
	skeletal_mesh.local = XMMATRIX(
		3, 3, 3, 3,
		3, 3, 3, 3,
		3, 3, 3, 3,
		3, 3, 3, 3
	);
	registry.emplace<SkeletalMesh>(entity_id_, skeletal_mesh);

	transform_tree_.root_node = make_shared<TransformTreeNode>(TYPE_ID(Transform));
	transform_tree_.root_node->children.push_back(make_shared<TransformTreeNode>(TYPE_ID(StaticMesh)));
	transform_tree_.root_node->children.push_back(make_shared<TransformTreeNode>(TYPE_ID(SkeletalMesh)));

	transform_tree_.root_node->OnUpdate(registry, entity_id_);
}

void KGCA41B::Actor::OnUpdate(entt::registry& registry)
{
	this->node_num_ = SpacePartition::GetInst()->UpdateNodeObjectBelongs(0, collision_box_, entity_id_);
	vector<int> node_to_search = SpacePartition::GetInst()->FindCollisionSearchNode(0, collision_box_);
	transform_tree_.root_node->OnUpdate(registry, entity_id_);

	std::unordered_set<entt::entity> object_to_collision_check;
	for (int node : node_to_search) {
		std::unordered_set<entt::entity> new_objects = SpacePartition::GetInst()->GetObjectListInNode(node);
		object_to_collision_check.insert(new_objects.begin(), new_objects.end());
	}
}
