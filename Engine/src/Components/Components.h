#pragma once
#include "stdafx.h"
#include "Shader.h"
#include "Texture.h"
#include "Collision.h"
#include "Material.h"
#include "UIBase.h"
#include "Sound.h"
#include "Vertex.h"
#include "Effect.h"
#include "DX11App.h"
#include "ResourceMgr.h"
#include "AnimSlot.h"
#include "Socket.h"

namespace reality
{
#define TYPE_ID(type) entt::type_hash<type>().value()

	struct Component
	{
		string tag;
		virtual void OnConstruct() {};
		virtual void OnDestroy() {};
		virtual void OnUpdate() {};
	};

	struct C_Transform : public Component
	{
		XMMATRIX local = XMMatrixIdentity();
		XMMATRIX world = XMMatrixIdentity();

		virtual void OnConstruct() override
		{
			local = XMMatrixIdentity();
			world = XMMatrixIdentity();
		}
	};

	struct C_Socket : public C_Transform
	{
		unordered_map<string, Socket> sockets;

		void AddSocket(string socket_name, string bone_name, XMMATRIX local_offset) {
			sockets.insert({ socket_name, Socket(bone_name, local_offset) });
		}
	};

	struct C_StaticMesh : public C_Transform
	{
		string static_mesh_id;
		string vertex_shader_id = "StaticMeshVS.cso";
		string socket_name = "";

		virtual void OnConstruct() override {};
	};

	struct C_SkeletalMesh : public C_Transform
	{

		string skeletal_mesh_id;
		string vertex_shader_id = "SkinningVS.cso";

		virtual void OnConstruct() override {};
	};

	struct C_CapsuleCollision : public C_Transform
	{
		reality::CapsuleShape capsule;
		UINT enclosed_node_index = 0;

		void SetCapsuleData(XMFLOAT3 offset, float height, float radius) {
			capsule = reality::CapsuleShape(offset, height, radius);
			local = XMMatrixTranslationFromVector(_XMVECTOR3(capsule.base));
			world = world * local;
		}
		virtual void OnUpdate() override
		{
			capsule.base = _XMFLOAT3(world.r[3]);
		}
	};

	struct C_Camera : public C_Transform
	{
		XMVECTOR camera_pos = { 0, 0, 0, 0 };
		XMVECTOR target_pos;
		float target_height = 0.0f;
		XMVECTOR local_pos;
		XMFLOAT2 pitch_yaw = { 0, 0 };
		XMVECTOR look, right, up;
		float rotation = 0.0f, target_rotation = 0.0f;
		float near_z, far_z, fov, aspect;

		virtual void OnConstruct() override {};
		virtual void OnUpdate() override
		{
			XMVECTOR target_translation, target_rotation, target_scale;
			XMVECTOR local_translation, local_rotation, local_scale;
			XMVECTOR camera_translation, camera_rotation, camera_scale;
			XMMatrixDecompose(&target_scale, &target_rotation, &target_pos, world);
			target_pos.m128_f32[1] += target_height;
			XMMatrixDecompose(&local_scale, &local_rotation, &local_pos, local);
			XMMatrixDecompose(&camera_scale, &camera_rotation, &camera_pos, local * world);
		}
		void SetLocalFrom(C_CapsuleCollision& capsule_collision, float arm_length)
		{
			local = XMMatrixTranslationFromVector(XMVectorSet(0, 1, -1, 0) * arm_length);
			target_height = capsule_collision.capsule.GetTipBaseAB()[0].m128_f32[1];
			pitch_yaw = { 0, 0};
			near_z = 1.f;
			far_z = 6000.f;
			fov = XMConvertToRadians(90);
			tag = "Player";
		}
	};

	struct C_Animation : public Component
	{
		vector<pair<string, AnimSlot>> anim_slots;
		unordered_map<string, int> name_to_anim_slot_index;

		C_Animation(AnimationBase* anim_object) {
			AnimSlot base_anim_slot;
			base_anim_slot.anim_object_ = make_shared<AnimationBase>(*anim_object);
			anim_slots.push_back({ "Base", base_anim_slot });
			name_to_anim_slot_index.insert({ "Base", 0 });
		}

		virtual void OnConstruct() override {};
		virtual void OnUpdate() override {};

		template<typename AnimSlotType, typename... Args>
		void AddNewAnimSlot(string anim_slot_name, string skeletal_mesh_id, string bone_id, int range, Args&&... args) {
			AnimSlot anim_slot;

			SkeletalMesh* skeletal_mesh = RESOURCE->UseResource<SkeletalMesh>(skeletal_mesh_id);

			skeletal_mesh->skeleton.GetSubBonesOf(bone_id, range, anim_slot.included_skeletons_, anim_slot.bone_id_to_weight_);
			anim_slot.range_ = range * 2;
			anim_slot.anim_object_ = make_shared<AnimSlotType>(args...);
			anim_slot.anim_object_->OnInit();

			anim_slots.push_back({ anim_slot_name, anim_slot });
			name_to_anim_slot_index.insert({ anim_slot_name, anim_slots.size() - 1 });
		};

		AnimSlot GetAnimSlotByName(string anim_slot_name) {
			return anim_slots[name_to_anim_slot_index[anim_slot_name]].second;
		}

		XMMATRIX GetCurAnimMatirixOfBone(int bone_id) {
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

			for (i;i >= 1;i--) {
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
	};

	struct C_SoundListener : public C_Transform
	{

	};

	struct C_SoundGenerator : public C_Transform
	{
		queue<SoundQueue> sound_queue_list;
	};

	struct C_BoundingBox : public C_Transform
	{
		AABBShape aabb;
		virtual void OnUpdate() override
		{
			XMMATRIX translation = XMMatrixTranslationFromVector(world.r[3]);

			world = translation;

			aabb.min = _XMFLOAT3(XMVector3TransformCoord(_XMVECTOR3(aabb.min), translation));
			aabb.max = _XMFLOAT3(XMVector3TransformCoord(_XMVECTOR3(aabb.max), translation));
			aabb.center = _XMFLOAT3(((_XMVECTOR3(aabb.min) + _XMVECTOR3(aabb.max)) / 2));
		}

		string vs_id = "StaticMeshVS.cso";

		vector<Vertex>			vertex_list;
		ComPtr<ID3D11Buffer>	vertex_buffer;

		vector<DWORD>			index_list;
		ComPtr<ID3D11Buffer>	index_buffer;

		int x, y, z;

		void SetXYZ(int x, int y, int z) {
			vertex_list.clear();
			vertex_list.push_back({ XMFLOAT3(-x / 2,  y, -z / 2),  XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) });  // Top left
			vertex_list.push_back({ XMFLOAT3(x / 2,  y, -z / 2),   XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) });   // Top right
			vertex_list.push_back({ XMFLOAT3(-x / 2, 0, -z / 2), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) });  // Bottom left
			vertex_list.push_back({ XMFLOAT3(x / 2, 0, -z / 2),  XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) });   // Bottom right
			vertex_list.push_back({ XMFLOAT3(-x / 2,  y,  z / 2),   XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) });   // Top front
			vertex_list.push_back({ XMFLOAT3(x / 2,  y,  z / 2),    XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) });    // Top back
			vertex_list.push_back({ XMFLOAT3(-x / 2, 0,  z / 2),  XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) });  // Bottom front
			vertex_list.push_back({ XMFLOAT3(x / 2, 0,  z / 2),   XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) });   // Bottom back

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

			index_list.clear();
			index_list.push_back(0);
			index_list.push_back(1);
			index_list.push_back(2);
			index_list.push_back(1);
			index_list.push_back(3);
			index_list.push_back(2);

			index_list.push_back(4);
			index_list.push_back(5);
			index_list.push_back(6);
			index_list.push_back(5);
			index_list.push_back(7);
			index_list.push_back(6);

			index_list.push_back(4);
			index_list.push_back(6);
			index_list.push_back(2);
			index_list.push_back(4);
			index_list.push_back(2);
			index_list.push_back(0);

			index_list.push_back(1);
			index_list.push_back(5);
			index_list.push_back(3);
			index_list.push_back(5);
			index_list.push_back(7);
			index_list.push_back(3);

			index_list.push_back(4);
			index_list.push_back(0);
			index_list.push_back(5);
			index_list.push_back(5);
			index_list.push_back(0);
			index_list.push_back(1);

			index_list.push_back(2);
			index_list.push_back(3);
			index_list.push_back(6);
			index_list.push_back(3);
			index_list.push_back(7);
			index_list.push_back(6);

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

			aabb.min = { -static_cast<float>(x) / 2, static_cast<float>(y), -static_cast<float>(z) / 2 };
			aabb.min = { static_cast<float>(x) / 2, -static_cast<float>(y), static_cast<float>(z) / 2 };
		}
	};

	struct TransformTreeNode
	{
		entt::id_type id_type;
		weak_ptr<TransformTreeNode> parent;
		vector<shared_ptr<TransformTreeNode>> children;

		TransformTreeNode() {};
		TransformTreeNode(entt::id_type type) : id_type(type) {};

		void OnUpdate(entt::registry& registry, entt::entity entity, XMMATRIX world = XMMatrixIdentity()) {
			C_Transform* cur_transform = static_cast<C_Transform*>(registry.storage(id_type)->get(entity));
			cur_transform->world = world;
			cur_transform->OnUpdate();

			for (auto child : children) {
				child->OnUpdate(registry, entity, world * cur_transform->local);
			}
		}

		void Rotate(entt::registry& registry, entt::entity entity, XMVECTOR rotation_center, XMMATRIX rotation_matrix = XMMatrixIdentity()) {
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

		void Translate(entt::registry& registry, entt::entity entity, XMMATRIX world = XMMatrixIdentity()) {
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

		void FastenAtFloor(entt::registry& registry, entt::entity ent, XMVECTOR floor_pos)
		{
			XMMATRIX floor = XMMatrixTranslationFromVector(floor_pos);
			this->Translate(registry, ent, floor);
		}
	};

	struct TransformTree
	{
		shared_ptr<TransformTreeNode> root_node;

		bool AddNodeToNode(entt::id_type parent_id_type, entt::id_type child_id_type) {
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

		shared_ptr<TransformTreeNode> FindNode(shared_ptr<TransformTreeNode> parent_node, entt::id_type id_type) {
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
	};

	struct C_Movement : public Component
	{
		TransformTree* actor_transform_tree;
		
		XMVECTOR direction = { 0.0f, 0.0f, 0.0f, 0.0f };
		float speed = 0.0f;
		float max_speed = 0.0f;
		float acceleration = 0.0f;
		float rotation = 0.0f;
		float angular_velocity = 0.0f;

		float jump_scale = 0.0f;
		float gravity = 0.0f;

		C_Movement(TransformTree* actor_transform_tree) :
			actor_transform_tree(actor_transform_tree) {};

	};

	struct C_BoxShape : public C_Transform
	{
		string vs_id;
		string material_id;

		vector<Vertex>			vertex_list;
		ComPtr<ID3D11Buffer>	vertex_buffer;

		vector<DWORD>			index_list;
		ComPtr<ID3D11Buffer>	index_buffer;

		C_BoxShape()
		{
			material_id = "box_material.mat";

			// ���ؽ� ����
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

			// �ε��� ����

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
	};


	struct C_Effect : public C_Transform
	{
		string	effect_id;
		Effect	effect;
		float	effect_lifetime = -1.0f;
		float	effect_timer;
	};
  
	struct C_UI : public Component
	{
		map<string, shared_ptr<UIBase>> ui_list;
	};

	// Light
	struct C_PointLight : public C_Transform
	{
		float		timer;
		float		lifetime;
		string		point_light_id;
		PointLight	point_light;
	};

	struct C_SpotLight : public C_Transform
	{
		float		timer;
		float		lifetime;
		string		spot_light_id;
		SpotLight	spot_light;
	};
}
