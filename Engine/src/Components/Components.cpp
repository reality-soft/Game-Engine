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

reality::C_Animation::C_Animation(int num_of_bones)
{
	num_of_bones += 5;
	animation_matrices.resize(num_of_bones);
	prev_slot_cur_time_animation_matrices.resize(num_of_bones);
	prev_slot_prev_time_animation_matrices.resize(num_of_bones);
	cur_slot_cur_time_animation_matrices.resize(num_of_bones);
	cur_slot_prev_time_animation_matrices.resize(num_of_bones);
	time_weights.resize(num_of_bones);
	bone_weights.resize(num_of_bones);
}

void reality::C_Animation::OnUpdate()
{
	for (const auto& anim_slot_pair : anim_slots) {
		const auto& anim_slot = anim_slot_pair.second;
		
		anim_slot->AnimationUpdate();

		const unordered_map<UINT, XMMATRIX>* anim_slot_matrices = nullptr;
		const unordered_map<UINT, XMMATRIX>* anim_slot_prev_matrices = nullptr;
		const unordered_map<UINT, int>* weights = nullptr;
		float range = 0.0f;
		float time_weight = 0.0f;
		switch (anim_slot->GetCurAnimState()) {
		case ANIM_STATE::ANIM_STATE_NONE:
			break;
		case ANIM_STATE::ANIM_STATE_CUR_ONLY:
			anim_slot_matrices = anim_slot->GetAnimationMatrices();
			weights = anim_slot->GetWeights();
			range = anim_slot->GetRange();
			time_weight = anim_slot->GetCurAnimTime() / anim_slot->GetBlendTime();
			
			for (const auto& animation_matrice_pair_per_bone : *anim_slot_matrices) {
				const auto& bone_id = animation_matrice_pair_per_bone.first;
				const auto& anim_slot_matrix_for_bone = animation_matrice_pair_per_bone.second;
				
				bone_weights[bone_id] = weights->at(bone_id) / range;
				time_weights[bone_id] = time_weight;

				prev_slot_cur_time_animation_matrices[bone_id] = cur_slot_cur_time_animation_matrices[bone_id];
				cur_slot_cur_time_animation_matrices[bone_id] = anim_slot_matrix_for_bone;
			}

			break;
		case ANIM_STATE::ANIM_STATE_PREV_ONLY:
			anim_slot_prev_matrices = anim_slot->GetPrevAnimationMatrices();
			weights = anim_slot->GetWeights();
			range = anim_slot->GetRange();
			time_weight = anim_slot->GetCurAnimTime() / anim_slot->GetBlendTime();

			for (const auto& animation_matrice_pair_per_bone : *anim_slot_prev_matrices) {
				const auto& bone_id = animation_matrice_pair_per_bone.first;
				const auto& anim_slot_matrix_for_bone = animation_matrice_pair_per_bone.second;

				bone_weights[bone_id] = weights->at(bone_id) / range;
				time_weights[bone_id] = time_weight;

				prev_slot_prev_time_animation_matrices[bone_id] = cur_slot_prev_time_animation_matrices[bone_id];
				cur_slot_prev_time_animation_matrices[bone_id] = anim_slot_matrix_for_bone;
			}

			break;
		case ANIM_STATE::ANIM_STATE_CUR_PREV:
			anim_slot_matrices = anim_slot->GetAnimationMatrices();
			anim_slot_prev_matrices = anim_slot->GetPrevAnimationMatrices();
			weights = anim_slot->GetWeights();
			range = anim_slot->GetRange();
			time_weight = anim_slot->GetCurAnimTime() / anim_slot->GetBlendTime();

			for (const auto& animation_matrice_pair_per_bone : *anim_slot_matrices) {
				const auto& bone_id = animation_matrice_pair_per_bone.first;
				const auto& anim_slot_matrix_for_bone = animation_matrice_pair_per_bone.second;
				const auto& anim_slot_prev_matrix_for_bone = anim_slot_prev_matrices->at(bone_id);

				bone_weights[bone_id] = weights->at(bone_id) / range;
				time_weights[bone_id] = time_weight;

				prev_slot_prev_time_animation_matrices[bone_id] = cur_slot_prev_time_animation_matrices[bone_id];
				cur_slot_prev_time_animation_matrices[bone_id] = anim_slot_matrix_for_bone;

				prev_slot_cur_time_animation_matrices[bone_id] = cur_slot_cur_time_animation_matrices[bone_id];
				cur_slot_cur_time_animation_matrices[bone_id] = anim_slot_matrix_for_bone;
			}
			break;
		}
	}

	for (int i = 0;i < animation_matrices.size();i++) {
		animation_matrices[i] = (prev_slot_prev_time_animation_matrices[i] * bone_weights[i] + cur_slot_prev_time_animation_matrices[i] * (1.0f - bone_weights[i])) * (1 - time_weights[i]) +
								(prev_slot_cur_time_animation_matrices[i] * bone_weights[i] + cur_slot_cur_time_animation_matrices[i] * (1.0f - bone_weights[i])) * time_weights[i];
	}
}

AnimationBase* reality::C_Animation::GetAnimSlotByName(string anim_slot_name)
{
	return anim_slots[name_to_anim_slot_index[anim_slot_name]].second.get();
}

XMMATRIX reality::C_Animation::GetCurAnimMatirixOfBone(int bone_id)
{
	return animation_matrices[bone_id];
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

