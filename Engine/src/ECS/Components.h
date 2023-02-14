#pragma once
#include "entt.hpp"
#include "DataTypes.h"
#include "Shader.h"
#include "Texture.h"

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

	struct TransformTreeNode
	{
		entt::id_type type;
		weak_ptr<TransformTreeNode> parent;
		vector<shared_ptr<TransformTreeNode>> children;

		TransformTreeNode() {};
		TransformTreeNode(entt::id_type type) : type(type) {};

		void OnUpdate(entt::registry &registry, entt::entity entity, XMMATRIX world = XMMatrixIdentity()) {
			Transform* cur_transform = static_cast<Transform*>(registry.storage(type)->get(entity));
			cur_transform->world = world;

			for (auto child : children) {
				child->OnUpdate(registry, entity, world * cur_transform->local);
			}
		}
	};

	struct TransformTree
	{
		shared_ptr<TransformTreeNode> root_node;
	};
}