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
		XMMATRIX world_matrix;
		XMMATRIX view_matrix;
		XMMATRIX projection_matrix;

		CbTransform cb_transform;
		ComPtr<ID3D11Buffer> cb_buffer;
	};

	struct StaticMesh : public Transform
	{
		string mesh_id;
		string shader_id;
	};

	struct SkeletalMesh : public Component
	{

		string mesh_id;
		string shader_id;
	};

	struct Material : public Component
	{
		string shader_id;
		string texture_id;
	};

	struct Camera : public Component
	{
		XMVECTOR position, look, up, right, target;
		float yaw, pitch, roll, distance, speed;
		float near_z, far_z, fov, aspect;
	};

	struct Skeleton : public Component
	{
		string skeleton_id;
		CbSkeleton cb_skeleton;
		ComPtr<ID3D11Buffer> cb_buffer;
	};

	struct Animation : public Component
	{
		string anim_id;
	};

	struct InputMapping : public Component
	{
		vector<AxisType> axis_types;
		float axis_value[6] = { 0, };
		vector<ActionType> actions;
	};
}