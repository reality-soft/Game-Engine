#pragma once
#include "entt.hpp"
#include "../Engine/DataTypes.h"
#include "../Engine/Shader.h"
#include "../Engine/Texture.h"

namespace KGCA41B
{
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
		weak_ptr<Transform> parent;
		vector<weak_ptr<Transform>> children;

		virtual void OnConstruct() override
		{
			local = XMMatrixIdentity();
			world = XMMatrixIdentity();
		}

		virtual void OnUpdate() override
		{
		}
	};

	struct StaticMesh : public Transform
	{
		string mesh_id;
		string shader_id;

		virtual void OnConstruct() override {};
	};

	struct SkeletalMesh : public Component
	{

		string mesh_id;
		string shader_id;

		virtual void OnConstruct() override {};
	};

	struct Material : public Component
	{
		string shader_id;
		string texture_id;

		virtual void OnConstruct() override {};
	};

	struct Camera : public Component
	{
		XMVECTOR position, look, up, right, target;
		float yaw, pitch, roll, distance, speed;
		float near_z, far_z, fov, aspect;

		virtual void OnConstruct() override {};
	};

	struct Skeleton : public Component
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

	struct SoundListener : public Component
	{

	};

	struct SoundGenerator : public Component
	{
		queue<SoundQueue> sound_queue_list;
	};

	struct TransformTreeNode
	{
	};

	class TransformTree
	{
		TransformTreeNode root;
	};
}