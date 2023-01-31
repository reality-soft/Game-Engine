#pragma once
#include "entt.hpp"
#include "../Engine/DataTypes.h"
#include "../Engine/Shader.h"

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
		vector<SingleMesh> mesh_list_;
		VsDefault vs_default_;

		virtual void OnConstruct() override {

			for (auto& single_mesh : this->mesh_list_)
			{
				D3D11_BUFFER_DESC vertex_buffer_desc;
				D3D11_SUBRESOURCE_DATA vertex_buffer_data;

				ZeroMemory(&vertex_buffer_desc, sizeof(vertex_buffer_desc));
				ZeroMemory(&vertex_buffer_data, sizeof(vertex_buffer_data));

				vertex_buffer_desc.ByteWidth = sizeof(Vertex) * single_mesh.vertices.size();
				vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
				vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				vertex_buffer_data.pSysMem = single_mesh.vertices.data();

				if (FAILED(DX11App::GetInst()->GetDevice()->CreateBuffer(&vertex_buffer_desc, &vertex_buffer_data, &single_mesh.vertex_buffer)))
				{
					MessageBox(nullptr, L"���ؽ� ���� �� ����", L"���", 0);
					assert(nullptr);
				}
			}
		}
	};

	struct SkeletalMesh : public Component
	{
		map<UINT, XMMATRIX> bindposes_;
		CbSkeleton bone_data_;
		ComPtr<ID3D11Buffer> bone_buffer_;

		virtual void OnConstruct() override {
			for (int i = 0; i < 255; ++i)
				this->bone_data_.mat_skeleton[i] = XMMatrixIdentity();

			D3D11_BUFFER_DESC bufferDesc;
			D3D11_SUBRESOURCE_DATA subresourceData;

			ZeroMemory(&bufferDesc, sizeof(bufferDesc));
			ZeroMemory(&subresourceData, sizeof(subresourceData));

			bufferDesc.ByteWidth = sizeof(CbSkeleton);
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			subresourceData.pSysMem = this->bone_buffer_.GetAddressOf();

			if (FAILED(DX11App::GetInst()->GetDevice()->CreateBuffer(&bufferDesc, &subresourceData, this->bone_buffer_.GetAddressOf())))
			{
				MessageBox(nullptr, L"���̷��� ��� ���� �� ����", L"���", 0);
				assert(nullptr);
			}
		}
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
		vector<AxisType> axis_types;
		float axis_value[6] = { 0, };
		vector<ActionType> actions;
	};
}