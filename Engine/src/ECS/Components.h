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
		XMMATRIX world_matrix_;
		XMMATRIX view_matrix_;
		XMMATRIX projection_matrix_;

		CbTransform cb_transform_;
		ComPtr<ID3D11Buffer> cb_buffer_;

		virtual void OnConstruct() override {

			this->world_matrix_ = this->cb_transform_.world_matrix = XMMatrixIdentity();
			this->world_matrix_ = this->cb_transform_.world_matrix = XMMatrixIdentity();
			this->world_matrix_ = this->cb_transform_.world_matrix = XMMatrixIdentity();

			D3D11_BUFFER_DESC buffer_desc;
			D3D11_SUBRESOURCE_DATA subresource_data;

			ZeroMemory(&buffer_desc, sizeof(buffer_desc));
			ZeroMemory(&subresource_data, sizeof(subresource_data));

			buffer_desc.ByteWidth = sizeof(CbTransform);
			buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;


			subresource_data.pSysMem = &this->cb_transform_;
			if (FAILED(DX11App::GetInst()->GetDevice()->CreateBuffer(&buffer_desc, &subresource_data, &this->cb_buffer_)))
			{
				MessageBox(nullptr, L"��� ���� �� ����", L"���", 0);
				assert(nullptr);
			}
		}
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

	struct ObjectComponent
	{
		string mesh_ID;
		string meterial_ID;
		vector<SingleMesh<Vertex>>* mesh_list;
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