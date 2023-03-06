#pragma once
#include "entt.hpp"
#include "DataTypes.h"
#include "Shader.h"
#include "Texture.h"
#include "Shape.h"
#include "Material.h"


namespace KGCA41B
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
		XMMATRIX local;
		XMMATRIX world;

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

	struct C_Camera : public C_Transform
	{
		XMVECTOR camera_pos = { 0, 0, 0, 0 };
		XMVECTOR target_pos;
		XMVECTOR local_pos;
		XMFLOAT2 pitch_yaw = { 0, 0 };
		float near_z, far_z, fov, aspect;

		virtual void OnConstruct() override {};
		virtual void OnUpdate() override
		{
			XMVECTOR target_translation, target_rotation, target_scale;
			XMVECTOR local_translation, local_rotation, local_scale;
			XMVECTOR camera_translation, camera_rotation, camera_scale;
			XMMatrixDecompose(&target_scale, &target_rotation, &target_pos, world);
			XMMatrixDecompose(&local_scale, &local_rotation, &local_pos, local);
			XMMatrixDecompose(&camera_scale, &camera_rotation, &camera_pos, world * local);
		}
	};

	struct C_Animation : public Component
	{
		string anim_id;

		virtual void OnConstruct() override {};
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

		void ApplyMovement(entt::registry& registry, entt::entity entity, XMMATRIX world = XMMatrixIdentity()) {
			C_Transform* cur_transform = static_cast<C_Transform*>(registry.storage(id_type)->get(entity));
			cur_transform->world = world;
			cur_transform->OnUpdate();

			
			XMVECTOR local_scale, local_rotation, local_translation;
			XMMatrixDecompose(&local_scale, &local_rotation, &local_translation, cur_transform->local);
			
			XMMATRIX translation_matrix = XMMatrixTranslationFromVector(local_translation);
			
			for (auto child : children) {
				child->ApplyMovement(registry, entity, world * translation_matrix);
			}
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
		float gravity_scale = 0.0f;
		float rotation = 0.0f;
		float angular_velocity = 0.0f;

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
		vector<shared_ptr<Emitter>> emitters;
	};

	struct PhysicsCollision : public C_Transform
	{
		reactphysics3d::CollisionShape* shape = nullptr;
		reactphysics3d::Collider* collider = nullptr;
		reactphysics3d::CollisionBody* body = nullptr;
	};
}
