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
		XMVECTOR position, look, up, right, target;
		float yaw, pitch, roll, distance, speed;
		float near_z, far_z, fov, aspect;

		virtual void OnConstruct() override {};
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

			aabb.min = XMVector3TransformCoord(aabb.min, translation);
			aabb.max = XMVector3TransformCoord(aabb.max, translation);
			aabb.center = (aabb.min + aabb.max) / 2;
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


	struct C_BoxShape : public C_Transform
	{
		string vs_id;

		vector<Vertex>			vertex_list;
		ComPtr<ID3D11Buffer>	vertex_buffer;

		vector<DWORD>			index_list;
		ComPtr<ID3D11Buffer>	index_buffer;

		C_BoxShape()
		{
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
		vector<Emitter> emitters;
	};

	struct PhysicsCollision : public C_Transform
	{
		reactphysics3d::CollisionShape* shape = nullptr;
		reactphysics3d::Collider* collider = nullptr;
		reactphysics3d::CollisionBody* body = nullptr;
	};
}
