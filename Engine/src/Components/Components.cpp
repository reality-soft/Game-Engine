#include "stdafx.h"
#include "Components.h"

using namespace reality;

void reality::C_Transform::OnConstruct()
{
	local = XMMatrixIdentity();
	world = XMMatrixIdentity();
}

void reality::C_CapsuleCollision::SetCapsuleData(XMFLOAT3 offset, float height, float radius)
{
	capsule = reality::CapsuleShape(offset, height, radius);
	local = XMMatrixTranslationFromVector(_XMVECTOR3(capsule.base));
	world = world * local;
}

void reality::C_CapsuleCollision::OnUpdate()
{
	capsule.base = _XMFLOAT3(world.r[3]);
}

void reality::C_SphereCollision::SetSphereData(XMFLOAT3 offset, float radius)
{
	sphere = reality::SphereShape(offset, radius);
	is_collide = 0;
	tri_normal = { 0.0f, 0.0f, 0.0f };
	local = XMMatrixTranslationFromVector(_XMVECTOR3(sphere.center));
	world = world * local;
}

void reality::C_SphereCollision::OnUpdate()
{
	sphere.center = _XMFLOAT3(world.r[3]);
}

void reality::C_TriggerVolume::OnUpdate()
{
	sphere_volume.center = _XMFLOAT3(world.r[3]);
}


void reality::C_Camera::OnUpdate()
{
	XMVECTOR target_translation, target_rotation, target_scale;
	XMVECTOR local_translation, local_rotation, local_scale;
	XMVECTOR camera_translation, camera_rotation, camera_scale;
	XMMatrixDecompose(&target_scale, &target_rotation, &target_pos, world);
	target_pos.m128_f32[1] += target_height;
	XMMatrixDecompose(&local_scale, &local_rotation, &local_pos, local);
	XMMatrixDecompose(&camera_scale, &camera_rotation, &camera_pos, local * world);
}

void reality::C_Camera::SetLocalFrom(C_CapsuleCollision& capsule_collision, float arm_length)
{
	local = XMMatrixTranslationFromVector(XMVectorSet(0, 1, -1, 0) * arm_length);
	target_height = GetTipBaseAB(capsule_collision.capsule)[0].m128_f32[1];
	pitch_yaw = { 0, 0 };
	near_z = 1.f;
	far_z = 10000.f;
	fov = XMConvertToRadians(90);
	tag = "Player";
}

reality::C_Animation::C_Animation(AnimationBase* anim_object)
{
	AnimSlot base_anim_slot;
	base_anim_slot.anim_object_ = make_shared<AnimationBase>(*anim_object);
	anim_slots.push_back({ "Base", base_anim_slot });
	name_to_anim_slot_index.insert({ "Base", 0 });
}

AnimSlot reality::C_Animation::GetAnimSlotByName(string anim_slot_name)
{
	return anim_slots[name_to_anim_slot_index[anim_slot_name]].second;
}

XMMATRIX reality::C_Animation::GetCurAnimMatirixOfBone(int bone_id)
{
	const AnimSlot& base_anim_slot = anim_slots[0].second;
	ANIM_STATE base_anim_slot_state = base_anim_slot.GetCurAnimState();

	float base_time_weight = min(1.0f, base_anim_slot.anim_object_->GetCurAnimTime() / base_anim_slot.anim_object_->GetBlendTime());

	XMMATRIX base_cur_animation = XMMatrixIdentity();
	XMMATRIX base_prev_animation = XMMatrixIdentity();
	XMMATRIX base_animation = XMMatrixIdentity();

	OutAnimData* res_animation = nullptr;
	OutAnimData* res_prev_animation = nullptr;

	switch (base_anim_slot_state) {
	case ANIM_STATE::ANIM_STATE_NONE:
		return XMMatrixIdentity();
	case ANIM_STATE::ANIM_STATE_CUR_ONLY:
		res_animation = RESOURCE->UseResource<OutAnimData>(base_anim_slot.anim_object_->GetCurAnimationId());
		base_prev_animation = res_animation->animations[bone_id][base_anim_slot.anim_object_->GetCurFrame()];
		base_cur_animation = res_animation->animations[bone_id][base_anim_slot.anim_object_->GetCurFrame()];
		base_animation = base_cur_animation;
		break;
	case ANIM_STATE::ANIM_STATE_CUR_PREV:
		res_prev_animation = RESOURCE->UseResource<OutAnimData>(base_anim_slot.anim_object_->GetPrevAnimationId());
		res_animation = RESOURCE->UseResource<OutAnimData>(base_anim_slot.anim_object_->GetCurAnimationId());

		base_prev_animation = res_prev_animation->animations[bone_id][base_anim_slot.anim_object_->GetPrevAnimLastFrame()];
		base_cur_animation = res_animation->animations[bone_id][base_anim_slot.anim_object_->GetCurFrame()];

		base_animation = base_cur_animation * base_time_weight + base_prev_animation * (1.0f - base_time_weight);
		break;
	}

	int i = anim_slots.size() - 1;
	float slot_weight = 0.0f;

	XMMATRIX slot_cur_animation = XMMatrixIdentity();
	XMMATRIX slot_prev_animation = XMMatrixIdentity();
	XMMATRIX slot_animation = XMMatrixIdentity();

	OutAnimData* res_prev_slot_animation = nullptr;
	OutAnimData* res_slot_animation = nullptr;

	for (i; i >= 1; i--) {
		AnimSlot& anim_slot = anim_slots[i].second;
		ANIM_STATE slot_anim_state = anim_slot.GetCurAnimState();

		float slot_time_weight = min(1.0f, anim_slot.anim_object_->GetCurAnimTime() / anim_slot.anim_object_->GetBlendTime());

		slot_cur_animation = XMMatrixIdentity();
		slot_prev_animation = XMMatrixIdentity();
		slot_animation = XMMatrixIdentity();

		switch (slot_anim_state) {
		case ANIM_STATE::ANIM_STATE_NONE:
			continue;
		case ANIM_STATE::ANIM_STATE_PREV_ONLY:
			res_prev_slot_animation = RESOURCE->UseResource<OutAnimData>(anim_slot.anim_object_->GetPrevAnimationId());
			slot_prev_animation = res_prev_slot_animation->animations[bone_id][anim_slot.anim_object_->GetPrevAnimLastFrame()];

			slot_animation = base_cur_animation * slot_time_weight + slot_prev_animation * (1.0f - slot_time_weight);
			break;
		case ANIM_STATE::ANIM_STATE_CUR_ONLY:
			res_slot_animation = RESOURCE->UseResource<OutAnimData>(anim_slot.anim_object_->GetCurAnimationId());
			slot_cur_animation = res_slot_animation->animations[bone_id][anim_slot.anim_object_->GetCurFrame()];

			slot_animation = slot_cur_animation * slot_time_weight + base_prev_animation * (1.0f - slot_time_weight);
			break;
		case ANIM_STATE::ANIM_STATE_CUR_PREV:
			res_prev_slot_animation = RESOURCE->UseResource<OutAnimData>(anim_slot.anim_object_->GetPrevAnimationId());
			res_slot_animation = RESOURCE->UseResource<OutAnimData>(anim_slot.anim_object_->GetCurAnimationId());

			slot_prev_animation = res_slot_animation->animations[bone_id][anim_slot.anim_object_->GetPrevAnimLastFrame()];
			slot_cur_animation = res_slot_animation->animations[bone_id][anim_slot.anim_object_->GetCurFrame()];

			slot_animation = slot_cur_animation * slot_time_weight + slot_prev_animation * (1.0f - slot_time_weight);
		}

		slot_weight = anim_slot.bone_id_to_weight_[bone_id] / anim_slot.range_;
		break;
	}

	return base_animation * (1.0f - slot_weight) + slot_animation * slot_weight;
}

reality::C_BoxShape::C_BoxShape()
{
	material_id = "box_material.mat";

	vertex_list.push_back({ { -1.0f, +1.0f, +0.0f }, {+0.0f, +0.0f, +0.0f}, {+0.0f, +0.0f} });
	vertex_list.push_back({ { +1.0f, +1.0f, +0.0f }, {+0.0f, +0.0f, +0.0f}, {+1.0f, +0.0f} });
	vertex_list.push_back({ { -1.0f, -1.0f, +0.0f }, {+0.0f, +0.0f, +0.0f}, {+0.0f, +1.0f} });
	vertex_list.push_back({ { +1.0f, -1.0f, +0.0f }, {+0.0f, +0.0f, +0.0f}, {+1.0f, +1.0f} });

	D3D11_BUFFER_DESC bufDesc;

	ZeroMemory(&bufDesc, sizeof(D3D11_BUFFER_DESC));

	bufDesc.ByteWidth = sizeof(Vertex) * vertex_list.size();
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = 0;
	bufDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subResourse;

	ZeroMemory(&subResourse, sizeof(D3D11_SUBRESOURCE_DATA));

	subResourse.pSysMem = &vertex_list.at(0);
	subResourse.SysMemPitch;
	subResourse.SysMemSlicePitch;

	DX11APP->GetDevice()->CreateBuffer(&bufDesc, &subResourse, &vertex_buffer);

	index_list.push_back(0);
	index_list.push_back(1);
	index_list.push_back(2);
	index_list.push_back(2);
	index_list.push_back(1);
	index_list.push_back(3);

	ZeroMemory(&bufDesc, sizeof(D3D11_BUFFER_DESC));

	bufDesc.ByteWidth = sizeof(DWORD) * index_list.size();
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = 0;
	bufDesc.StructureByteStride = 0;

	ZeroMemory(&subResourse, sizeof(D3D11_SUBRESOURCE_DATA));

	subResourse.pSysMem = &index_list.at(0);
	subResourse.SysMemPitch;
	subResourse.SysMemSlicePitch;

	DX11APP->GetDevice()->CreateBuffer(&bufDesc, &subResourse, &index_buffer);
}

void reality::TransformTreeNode::OnUpdate(entt::registry& registry, entt::entity entity, XMMATRIX world)
{
	C_Transform* cur_transform = static_cast<C_Transform*>(registry.storage(id_type)->get(entity));
	cur_transform->world = world;
	cur_transform->OnUpdate();

	for (auto child : children) {
		child->OnUpdate(registry, entity, world * cur_transform->local);
	}
}

void reality::TransformTreeNode::Rotate(entt::registry& registry, entt::entity entity, XMVECTOR rotation_center, XMMATRIX rotation_matrix)
{
	C_Transform* cur_transform = static_cast<C_Transform*>(registry.storage(id_type)->get(entity));
	XMVECTOR world_scale, world_rotation, world_translation;
	XMMatrixDecompose(&world_scale, &world_rotation, &world_translation, cur_transform->world);

	if (id_type != TYPE_ID(C_Camera)) {
		cur_transform->world *= XMMatrixTranslationFromVector(-rotation_center);
		cur_transform->world *= XMMatrixInverse(0, XMMatrixRotationQuaternion(world_rotation));
		cur_transform->world *= rotation_matrix;
		cur_transform->world *= XMMatrixTranslationFromVector(rotation_center);
	}

	for (auto child : children) {
		child->Rotate(registry, entity, rotation_center, rotation_matrix);
	}
}

void reality::TransformTreeNode::Translate(entt::registry& registry, entt::entity entity, XMMATRIX world)
{
	C_Transform* cur_transform = static_cast<C_Transform*>(registry.storage(id_type)->get(entity));
	XMVECTOR world_scale, world_rotation, world_translation;
	XMMatrixDecompose(&world_scale, &world_rotation, &world_translation, cur_transform->world);

	cur_transform->world *= XMMatrixTranslationFromVector(-world_translation);
	cur_transform->world *= world;

	cur_transform->OnUpdate();

	XMVECTOR local_scale, local_rotation, local_translation;
	XMMatrixDecompose(&local_scale, &local_rotation, &local_translation, cur_transform->local);

	XMMATRIX translation_matrix = XMMatrixTranslationFromVector(local_translation);

	for (auto child : children) {
		child->Translate(registry, entity, world * translation_matrix);
	}
}

bool reality::TransformTree::AddNodeToNode(entt::id_type parent_id_type, entt::id_type child_id_type)
{
	auto parent_node = FindNode(root_node, parent_id_type);
	if (parent_node == nullptr) {
		return false;
	}
	else {
		parent_node->children.push_back(make_shared<TransformTreeNode>(child_id_type));
		parent_node->children[parent_node->children.size() - 1]->parent = weak_ptr<TransformTreeNode>(parent_node);
		return true;
	}
}

shared_ptr<TransformTreeNode> reality::TransformTree::FindNode(shared_ptr<TransformTreeNode> parent_node, entt::id_type id_type)
{
	if (parent_node->id_type == id_type) {
		return parent_node;
	}
	for (auto child : parent_node->children) {
		auto search_result = FindNode(child, id_type);
		if (search_result != nullptr) {
			return search_result;
		}
	}


	return nullptr;
}

