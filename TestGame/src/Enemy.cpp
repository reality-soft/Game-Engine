#include "Enemy.h"

using namespace reality;

void Enemy::OnInit(entt::registry& registry)
{
	Character::OnInit(registry);

	// setting character data
	movement_component_->speed = 100;
	max_hp_ = cur_hp_ = 100;

	SetCharacterAnimation("Zombie_Idle_1_v2_IPC_Anim_Unreal Take.anim");

	// setting character objects
	reality::C_SkeletalMesh skm;
	skm.local = XMMatrixIdentity();
	skm.world = XMMatrixIdentity();
	skm.skeletal_mesh_id = "Zombie_Businessman_Male_01.skmesh";
	skm.vertex_shader_id = "SkinningVS.cso";
	registry.emplace_or_replace<reality::C_SkeletalMesh>(entity_id_, skm);

	reality::C_CapsuleCollision capsule;
	capsule.tag = "Enemy";
	capsule.SetCapsuleData(XMVectorZero(), 50, 10);
	registry.emplace<reality::C_CapsuleCollision>(entity_id_, capsule);

	transform_tree_.root_node = make_shared<TransformTreeNode>(TYPE_ID(reality::C_CapsuleCollision));
	transform_tree_.AddNodeToNode(TYPE_ID(C_CapsuleCollision), TYPE_ID(C_SkeletalMesh));

	transform_matrix_ = XMMatrixTranslationFromVector({ 0.f, 100.f, 0.f, 0.f });
	transform_tree_.root_node->OnUpdate(registry, entity_id_, transform_matrix_);

	reality::C_SkeletalMesh* skm_ptr = registry.try_get<C_SkeletalMesh>(entity_id_);
	skm_ptr->local = XMMatrixScalingFromVector({ 0.3, 0.3, 0.3, 0.0 }) * XMMatrixRotationY(XMConvertToRadians(180.f));

	// setting a character into quad tree
	QUADTREE->RegistDynamicCapsule(entity_id_);
}

void Enemy::OnUpdate()
{
	behavior_tree_.Update();
}

void Enemy::SetCharacterAnimation(string anim_id) const
{
	C_Animation* prev_animation = reg_scene_->try_get<reality::C_Animation>(entity_id_);
	if (prev_animation != nullptr && prev_animation->anim_id == anim_id) {
		return;
	}
	C_Animation animation;
	animation.anim_id = anim_id;
	reg_scene_->emplace_or_replace<reality::C_Animation>(entity_id_, animation);
}

void Enemy::Move()
{
}

void Enemy::Jump()
{
}

void Enemy::Idle()
{
}

void Enemy::Attack()
{
}

int Enemy::GetMaxHp() const
{
	return 0;
}

void Enemy::SetCurHp(int hp)
{
}

void Enemy::TakeDamage(int damage)
{
}

void Enemy::SetDirection(const XMVECTOR& direction)
{
	movement_component_->direction = direction;
}

void Enemy::SetRoute(const vector<XMVECTOR>& target_poses)
{
	SetPos(target_poses[0] + XMVECTOR{0, 1000.0f, 0, 0});
	// setting behavior tree
	behavior_tree_.SetRootNode<SequenceNode>();

	for (auto target_pos : target_poses) {
		BehaviorNode* root_node = behavior_tree_.GetRootNode();
		root_node->AddChild<EnemyMoveToTargets>(entity_id_, target_pos);
	}
}

void Enemy::SetMeshId(const string& mesh_id)
{
	C_SkeletalMesh *skm = reg_scene_->try_get< reality::C_SkeletalMesh>(entity_id_);
	skm->skeletal_mesh_id = mesh_id;
}

int Enemy::GetCurHp() const
{
	return 0;
}
