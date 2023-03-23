#include "Player.h"
#include "Weapon.h"

using namespace reality;

void Player::OnInit(entt::registry& registry)
{
	Character::OnInit(registry);

	movement_component_->speed = 500;
	max_hp_ = cur_hp_ = 100;

	SetCharacterAnimation("A_TP_CH_Breathing_Anim_Unreal Take.anim");

	reality::C_SkeletalMesh skm;
	skm.local = XMMatrixIdentity();
	skm.world = XMMatrixIdentity();
	skm.skeletal_mesh_id = "A_TP_CH_Breathing.skmesh";
	skm.vertex_shader_id = "SkinningVS.cso";
	registry.emplace_or_replace<reality::C_SkeletalMesh>(entity_id_, skm);

	reality::C_CapsuleCollision capsule;
	capsule.SetCapsuleData(XMVectorZero(), 50, 15);
	registry.emplace<reality::C_CapsuleCollision>(entity_id_, capsule);

	C_Camera camera;
	camera.SetLocalFrom(capsule, 50);
	registry.emplace<C_Camera>(entity_id_, camera);

	C_SoundListener sound_listener;
	sound_listener.local = camera.local;
	registry.emplace<C_SoundListener>(entity_id_, sound_listener);


	transform_tree_.root_node = make_shared<TransformTreeNode>(TYPE_ID(reality::C_CapsuleCollision));
	transform_tree_.AddNodeToNode(TYPE_ID(C_CapsuleCollision), TYPE_ID(C_SkeletalMesh));
	transform_tree_.AddNodeToNode(TYPE_ID(C_CapsuleCollision), TYPE_ID(C_Camera));
	transform_tree_.AddNodeToNode(TYPE_ID(C_CapsuleCollision), TYPE_ID(C_SoundListener));

	transform_matrix_ = XMMatrixTranslation(0, 100, 0);
	transform_tree_.root_node->OnUpdate(registry, entity_id_, transform_matrix_);

	reality::C_SkeletalMesh* skm_ptr = registry.try_get<C_SkeletalMesh>(entity_id_);
	skm_ptr->local = XMMatrixRotationY(XMConvertToRadians(180)) * XMMatrixScalingFromVector({ 0.3, 0.3, 0.3, 0.0 });

	// weapon
	entt::entity weapon_id = SCENE_MGR->AddActor<Weapon>(entity_id_);
	SkeletalMesh* skeletal_mesh = RESOURCE->UseResource<SkeletalMesh>(skm.skeletal_mesh_id);
	int skeleton_id = skeletal_mesh->skeleton.skeleton_id_map["hand_r"];
	Weapon* weapon = SCENE_MGR->GetActor<Weapon>(weapon_id);
	weapon->SetSocket(skeleton_id);
	weapon->SetOwnerTransform(skm_ptr->local);
}

void Player::OnUpdate()
{
	C_Camera* camera = reg_scene_->try_get<C_Camera>(entity_id_);
	XMVECTOR scale, rotation, translation;
	XMMatrixDecompose(&scale, &rotation, &translation, transform_matrix_);
	XMMATRIX rotation_matrix = XMMatrixRotationY(camera->pitch_yaw.y);
	transform_tree_.root_node->Rotate(*reg_scene_, entity_id_, translation, rotation_matrix);
	front_ = XMVector3Transform({ 0, 0, 1, 0 }, rotation_matrix);
	right_ = XMVector3Transform({ 1, 0, 0, 0 }, rotation_matrix);
}

void Player::SetCharacterAnimation(string anim_id)
{
	C_Animation* prev_animation = reg_scene_->try_get<reality::C_Animation>(entity_id_);
	if (prev_animation != nullptr && prev_animation->anim_id == anim_id) {
		return;
	}
	C_Animation animation;
	animation.anim_id = anim_id;
	reg_scene_->emplace_or_replace<reality::C_Animation>(entity_id_, animation);
}

void Player::MoveRight()
{
	SetCharacterAnimation("A_TP_CH_Jog_RF_Anim_Unreal Take.anim");
	movement_component_->direction += right_;
}

void Player::MoveRightForward()
{
	SetCharacterAnimation("A_TP_CH_Jog_RF_Anim_Unreal Take.anim");
	movement_component_->direction += front_;
	movement_component_->direction += right_;
}

void Player::MoveRightBack()
{
	SetCharacterAnimation("A_TP_CH_Jog_RB_Anim_Unreal Take.anim");
	movement_component_->direction -= front_;
	movement_component_->direction += right_;
}

void Player::MoveLeft()
{
	SetCharacterAnimation("A_TP_CH_Jog_LF_Anim_Unreal Take.anim");
	movement_component_->direction -= right_;
}

void Player::MoveLeftForward()
{
	SetCharacterAnimation("A_TP_CH_Jog_LF_Anim_Unreal Take.anim");
	movement_component_->direction += front_;
	movement_component_->direction -= right_;
}

void Player::MoveLeftBack()
{
	SetCharacterAnimation("A_TP_CH_Jog_LB_Anim_Unreal Take.anim");
	movement_component_->direction -= front_;
	movement_component_->direction -= right_;
}

void Player::MoveForward()
{
	SetCharacterAnimation("A_TP_CH_Jog_F_Anim_Unreal Take.anim");
	movement_component_->direction += front_;
}

void Player::MoveBack()
{
	SetCharacterAnimation("A_TP_CH_Jog_B_Anim_Unreal Take.anim");
	movement_component_->direction -= front_;
}

void Player::Jump()
{
	movement_component_->jump_scale = 1000.0f;
	movement_state_ = MovementState::GRAVITY_FALL;
}

void Player::Idle()
{
	SetCharacterAnimation("A_TP_CH_Breathing_Anim_Unreal Take.anim");
}

void Player::Fire()
{
	SetCharacterAnimation("A_TP_CH_Handgun_Fire_Anim_Unreal Take.anim");
}

void Player::ResetPos()
{
	transform_matrix_ = XMMatrixTranslation(0, 100, 0);
	transform_tree_.root_node->OnUpdate(SCENE_MGR->GetRegistry(), entity_id_, transform_matrix_);
}

int Player::GetMaxHp() const
{
	return max_hp_;
}

void Player::SetCurHp(int hp)
{
	cur_hp_ = hp;
}

void Player::TakeDamage(int damage)
{
	cur_hp_ -= damage;
}

int Player::GetCurHp() const
{
	return cur_hp_;
}
