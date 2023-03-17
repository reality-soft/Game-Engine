#include "Player.h"

using namespace reality;

void Player::OnInit(entt::registry& registry)
{
	Character::OnInit(registry);

	movement_component_->speed = 150;

	SetCharacterAnimation("A_TP_CH_Breathing_Anim.anim");

	reality::C_SkeletalMesh skm;
	skm.local = XMMatrixIdentity();
	skm.world = XMMatrixIdentity();
	skm.skeletal_mesh_id = "A_TP_CH_Breathing.skmesh";
	skm.vertex_shader_id = "SkinningVS.cso";
	auto& meshes = RESOURCE->UseResource<SkeletalMesh>(skm.skeletal_mesh_id)->meshes;
	registry.emplace_or_replace<reality::C_SkeletalMesh>(entity_id_, skm);

	reality::C_CapsuleCollision capsule;
	capsule.SetCapsuleData(XMVectorZero(), 50, 5);
	registry.emplace<reality::C_CapsuleCollision>(entity_id_, capsule);

	C_Camera camera;
	camera.SetLocalFrom(capsule, 50);
	registry.emplace<C_Camera>(entity_id_, camera);

	transform_tree_.root_node = make_shared<TransformTreeNode>(TYPE_ID(reality::C_CapsuleCollision));
	transform_tree_.AddNodeToNode(TYPE_ID(C_CapsuleCollision), TYPE_ID(C_SkeletalMesh));
	transform_tree_.AddNodeToNode(TYPE_ID(C_CapsuleCollision), TYPE_ID(C_Camera));

	transform_tree_.root_node->OnUpdate(registry, entity_id_, transform_matrix_);

	reality::C_SkeletalMesh* skm_ptr = registry.try_get<C_SkeletalMesh>(entity_id_);
	skm_ptr->local = XMMatrixRotationY(XMConvertToRadians(180)) * XMMatrixScalingFromVector({ 0.3, 0.3, 0.3, 0.0 });
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
	C_Animation animation;
	animation.anim_id = anim_id;
	reg_scene_->emplace_or_replace<reality::C_Animation>(entity_id_, animation);
}

void Player::MoveRight()
{
	SetCharacterAnimation("A_TP_CH_Jog_RF_Anim.anim");
	movement_component_->direction += right_;
}

void Player::MoveRightForward()
{
	SetCharacterAnimation("A_TP_CH_Jog_RF_Anim.anim");
	movement_component_->direction += front_;
	movement_component_->direction += right_;
}

void Player::MoveRightBack()
{
	SetCharacterAnimation("A_TP_CH_Jog_RB_Anim.anim");
	movement_component_->direction -= front_;
	movement_component_->direction += right_;
}

void Player::MoveLeft()
{
	SetCharacterAnimation("A_TP_CH_Jog_LF_Anim.anim");
	movement_component_->direction -= right_;
}

void Player::MoveLeftForward()
{
	SetCharacterAnimation("A_TP_CH_Jog_LF_Anim.anim");
	movement_component_->direction += front_;
	movement_component_->direction -= right_;
}

void Player::MoveLeftBack()
{
	SetCharacterAnimation("A_TP_CH_Jog_LB_Anim.anim");
	movement_component_->direction -= front_;
	movement_component_->direction -= right_;
}

void Player::MoveForward()
{
	SetCharacterAnimation("A_TP_CH_Jog_F_Anim.anim");
	movement_component_->direction += front_;
}

void Player::MoveBack()
{
	SetCharacterAnimation("A_TP_CH_Jog_B_Anim.anim");
	movement_component_->direction -= front_;
}

void Player::Idle()
{
	SetCharacterAnimation("A_TP_CH_Breathing_Anim.anim");
}

void Player::Fire()
{
	SetCharacterAnimation("A_TP_CH_Handgun_Fire_Anim.anim");
}