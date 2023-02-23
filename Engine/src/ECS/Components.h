#pragma once
#include "entt.hpp"
#include "DataTypes.h"
#include "Shader.h"
#include "Texture.h"
#include "Shape.h"


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

	struct Transform : public Component
	{
		XMMATRIX local;
		XMMATRIX world;

		virtual void OnConstruct() override
		{
			local = XMMatrixIdentity();
			world = XMMatrixIdentity();
		}
	};

	struct StaticMesh : public Transform
	{
		string mesh_id;
		string shader_id;

		virtual void OnConstruct() override {};
	};

	struct SkeletalMesh : public Transform
	{

		string mesh_id;
		string shader_id;

		virtual void OnConstruct() override {};
	};

	struct Material : public Component
	{
		string shader_id;
		vector<string> texture_id;

		virtual void OnConstruct() override {};
	};

	struct Camera : public Transform
	{
		XMVECTOR position, look, up, right, target;
		float yaw, pitch, roll, distance, speed;
		float near_z, far_z, fov, aspect;

		virtual void OnConstruct() override {};
	};

	struct Skeleton : public Transform
	{
		string skeleton_id;

		virtual void OnConstruct() override {};
	};

	struct Animation : public Component
	{
		string anim_id;

		virtual void OnConstruct() override {};
	};

	struct InputMapping : public Component
	{
		vector<AxisType> axis_types;
		float axis_value[6] = { 0, };
		vector<ActionType> actions;

		virtual void OnConstruct() override {};
	};

	struct SoundListener : public Transform
	{

	};

	struct SoundGenerator : public Transform
	{
		queue<SoundQueue> sound_queue_list;
	};

	struct BoundingBox : public Transform
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
			Transform* cur_transform = static_cast<Transform*>(registry.storage(id_type)->get(entity));
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

	struct Particle
	{
		bool		enable;
		string		tex_id;
		XMFLOAT3	position;
		XMFLOAT3	velocity;
		float		duration;
		float		timer;
		XMFLOAT4	color;
		Particle()
		{
			enable = true;
			position = {0, 0, 0};
			velocity = { 0, 0, 0 };
			duration = 3.0f;
			timer = 0.0f;
			color = {0.0f, 0.0f, 0.0f, 0.0f};
		}
	};


	struct BaseEffect : public Transform
	{
		bool					enabled_ = false;
		string					vs_id;
		string					ps_id;
		vector<Vertex>			vertex_list;
		ComPtr<ID3D11Buffer>	vertex_buffer;
	};

	struct UVSprite : public BaseEffect
	{
		string						tex_id;
		UINT						cur_frame = 1;
		UINT						max_frame;
		vector<UINT>				index_list;
		ComPtr<ID3D11Buffer>		index_buffer;
		vector<pair<POINT, POINT>>	uv_list;
	};

	struct TextureSprite : public BaseEffect
	{
		UINT						cur_frame = 1;
		UINT						max_frame;
		vector<UINT>				index_list;
		ComPtr<ID3D11Buffer>		index_buffer;
		vector<string>				tex_id_list;
	};

	struct Particles : public BaseEffect
	{
		string				geo_id;
		vector<string>		tex_id_list;
		UINT				particle_count;
		vector<Particle>	particle_list;
	};

	struct PhysicsCollision : public Transform
	{
		reactphysics3d::CollisionShape* shape = nullptr;
		reactphysics3d::Collider* collider = nullptr;
		reactphysics3d::CollisionBody* body = nullptr;
	};
}