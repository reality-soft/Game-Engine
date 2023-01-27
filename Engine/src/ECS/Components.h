#pragma once
#include "entt.hpp"
#include "../Engine/DataTypes.h"
#include "../Engine/Shader.h"

namespace KGCA41B
{
	struct Component
	{
		string tag;
	};

	struct Transform : public Component
	{
		XMMATRIX world_matrix;
		XMMATRIX view_matrix;
		XMMATRIX projection_matrix;

		CbTransform cb_transform;
		ComPtr<ID3D11Buffer> cb_buffer;
	};

	struct StaticMesh : public Component
	{
		vector<SingleMesh> mesh_list;
		VsDefault vs_default;
	};

	struct SkeletalMesh : public Component
	{
		vector<SingleMesh> mesh_list;
		VsSkinned vs_skinned;
	};

	struct Material : public Component
	{
		PsDefault ps_default;
		vector<ID3D11ShaderResourceView*> srv_list;
	};

	struct Camera : public Component
	{
		XMVECTOR position, look, up, right, target;
		float yaw, pitch, roll, distance, speed;
		float near_z, far_z, fov, aspect;
	};

	struct Skeleton : public Component
	{
		map<UINT, XMMATRIX> bind_poses;
		CbSkeleton cb_skeleton;
		ComPtr<ID3D11Buffer> cb_buffer;
	};

	struct Animation : public Component
	{
		map<UINT, vector<XMMATRIX>> anim_track;
		UINT start_frame = 0;
		UINT end_frame = 0;
	};

	struct InputMapping : public Component
	{
		float axis_value = 0; // -1 ~ 0 ~ +1
		vector<ActionType> actions;
	};
}