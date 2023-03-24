#include "Enemy.h"

using namespace reality;

void Enemy::OnInit(entt::registry& registry)
{
	Character::OnInit(registry);

	movement_component_->speed = 300;
	max_hp_ = cur_hp_ = 100;

	SetCharacterAnimation("Zombie_Idle_1_v2_IPC_Anim_Unreal Take.anim");

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


	QUADTREE->RegistDynamicCapsule(entity_id_);
}

void Enemy::OnUpdate()
{
	if (cur_target_pos_index_ >= target_poses_.size()) {
		SetCharacterAnimation("Zombie_Idle_1_v2_IPC_Anim_Unreal Take.anim");
		return;
	}
	if (XMVector3Length(GetPos() - target_poses_[cur_target_pos_index_]).m128_f32[0] <= 10.0f) {
		cur_target_pos_index_++;
		if (cur_target_pos_index_ >= target_poses_.size()) {
			return;
		}
	}

	SetCharacterAnimation("Zombie_Walk_F_6_Loop_IPC_Anim_Unreal Take.anim");

	movement_component_->direction = XMVector3Normalize(target_poses_[cur_target_pos_index_] - GetPos());
	
	XMVECTOR scale, rotation, translation;
	XMMatrixDecompose(&scale, &rotation, &translation, transform_matrix_);
	
	float dot_product = XMVectorGetX(XMVector3Dot(movement_component_->direction, { 0, 0, 1, 0 }));
	float magnitude1 = XMVectorGetX(XMVector3Length(movement_component_->direction));
	float magnitude2 = XMVectorGetX(XMVector3Length({ 0, 0, 1, 0 }));

	float radian_angle = acos(dot_product / (magnitude1 * magnitude2));

	if (dot_product < 0)
	{
		radian_angle = -radian_angle;
	}

	XMMATRIX rotation_matrix = XMMatrixRotationY(radian_angle);
	transform_tree_.root_node->Rotate(*reg_scene_, entity_id_, translation, rotation_matrix);
	front_ = XMVector3Transform({ 0, 0, 1, 0 }, rotation_matrix);
	right_ = XMVector3Transform({ 1, 0, 0, 0 }, rotation_matrix);

	//behavior_tree_.Execute();
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

void Enemy::SetPos(const XMVECTOR& position)
{
	transform_matrix_ = XMMatrixTranslationFromVector(position);
	transform_tree_.root_node->Translate(*reg_scene_, entity_id_, transform_matrix_);
}

void Enemy::SetDirection(const XMVECTOR& direction)
{
	movement_component_->direction = direction;
}

XMVECTOR Enemy::GetPos() const
{
	XMVECTOR scale, rotation, translation;
	XMMatrixDecompose(&scale, &rotation, &translation, transform_matrix_);

	return translation;
}

void Enemy::SetRoute(const vector<XMVECTOR>& target_poses)
{
	SetPos(target_poses[0]);

	target_poses_ = target_poses;

	// behavior_tree_.SetRootNode<EnemyMoveToTargets>(entity_id_, target_poses);
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
