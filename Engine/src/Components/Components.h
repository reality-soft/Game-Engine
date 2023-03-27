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

	struct C_StaticMesh : public C_Transform
	{
		string static_mesh_id;
		string vertex_shader_id = "StaticMeshVS.cso";

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

		void SetCapsuleData(XMVECTOR offset, float height, float radius) {
			capsule = reality::CapsuleShape(offset, height, radius);
			local = XMMatrixTranslationFromVector(capsule.base);
			world = world * local;
		}
		virtual void OnUpdate() override
		{
			capsule.base = world.r[3];
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
			far_z = 100000.f;
			fov = XMConvertToRadians(90);
			tag = "Player";
		}
	};

	struct C_Animation : public Component
	{
		string anim_id;
		float cur_frame = 0.0f;

		virtual void OnConstruct() override {};
		virtual void OnUpdate() override {
			OutAnimData* anim_data = RESOURCE->UseResource<OutAnimData>(anim_id);
			//cur_frame = anim_data->start_frame;
		};
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

			aabb.min = XMVector3TransformCoord(aabb.min, translation);
			aabb.max = XMVector3TransformCoord(aabb.max, translation);
			aabb.center = (aabb.min + aabb.max) / 2;
		}

		string vs_id = "StaticMeshVS.cso";

		vector<Vertex>			vertex_list;
		ComPtr<ID3D11Buffer>	vertex_buffer;

		vector<DWORD>			index_list;
		ComPtr<ID3D11Buffer>	index_buffer;

		int x, y, z;

		void SetXYZ(int x, int y, int z) {
			vertex_list.clear();
			vertex_list.push_back({ XMFLOAT3(-x / 2,  y, -z / 2),  XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) });  // Top left
			vertex_list.push_back({ XMFLOAT3(x / 2,  y, -z / 2),   XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) });   // Top right
			vertex_list.push_back({ XMFLOAT3(-x / 2, 0, -z / 2), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) });  // Bottom left
			vertex_list.push_back({ XMFLOAT3(x / 2, 0, -z / 2),  XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) });   // Bottom right
			vertex_list.push_back({ XMFLOAT3(-x / 2,  y,  z / 2),   XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) });   // Top front
			vertex_list.push_back({ XMFLOAT3(x / 2,  y,  z / 2),    XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) });    // Top back
			vertex_list.push_back({ XMFLOAT3(-x / 2, 0,  z / 2),  XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) });  // Bottom front
			vertex_list.push_back({ XMFLOAT3(x / 2, 0,  z / 2),   XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) });   // Bottom back

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

			aabb.min = { -static_cast<float>(x) / 2, static_cast<float>(y), -static_cast<float>(z) / 2, 0 };
			aabb.min = { static_cast<float>(x) / 2, -static_cast<float>(y), static_cast<float>(z) / 2, 0 };
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
			vertex_list.push_back({ { -1.0f, +1.0f, +0.0f }, {+0.0f, +0.0f, +0.0f}, {+1.0f, +1.0f, +1.0f, +1.0f}, {+0.0f, +0.0f} });
			vertex_list.push_back({ { +1.0f, +1.0f, +0.0f }, {+0.0f, +0.0f, +0.0f}, {+1.0f, +1.0f, +1.0f, +1.0f}, {+1.0f, +0.0f} });
			vertex_list.push_back({ { -1.0f, -1.0f, +0.0f }, {+0.0f, +0.0f, +0.0f}, {+1.0f, +1.0f, +1.0f, +1.0f}, {+0.0f, +1.0f} });
			vertex_list.push_back({ { +1.0f, -1.0f, +0.0f }, {+0.0f, +0.0f, +0.0f}, {+1.0f, +1.0f, +1.0f, +1.0f}, {+1.0f, +1.0f} });

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

	struct C_BaseLight : public C_Transform
	{
		float		timer;
		float		lifetime;
		XMFLOAT3	position;
		float		range;
		XMFLOAT3	attenuation;
	};

	// Light
	struct C_PointLight : public C_BaseLight
	{

	};

	struct C_SpotLight : public C_BaseLight
	{
		XMFLOAT3	direction;
		float		spot;
	};
}
