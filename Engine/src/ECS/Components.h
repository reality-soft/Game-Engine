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

		virtual void OnConstruct() override {
			this->world_matrix = this->cb_transform.world_matrix = XMMatrixIdentity();
			this->view_matrix = this->cb_transform.world_matrix = XMMatrixIdentity();
			this->projection_matrix = this->cb_transform.world_matrix = XMMatrixIdentity();

			D3D11_BUFFER_DESC desc;
			D3D11_SUBRESOURCE_DATA subdata;

			ZeroMemory(&desc, sizeof(desc));
			ZeroMemory(&subdata, sizeof(subdata));

			desc.ByteWidth = sizeof(CbTransform);

			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

			subdata.pSysMem = &this->cb_transform;
			HRESULT hr;
			hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, &this->cb_buffer);
		};
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
		CbSkeleton cb_skeleton;
		ComPtr<ID3D11Buffer> cb_buffer;

		virtual void OnConstruct() override {
			for (int i = 0; i < 255; ++i)
				this->cb_skeleton.mat_skeleton[i] = XMMatrixIdentity();

			D3D11_BUFFER_DESC desc;
			D3D11_SUBRESOURCE_DATA subdata;

			ZeroMemory(&desc, sizeof(desc));
			ZeroMemory(&subdata, sizeof(subdata));

			desc.ByteWidth = sizeof(CbSkeleton);
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			subdata.pSysMem = this->cb_buffer.GetAddressOf();

			HRESULT hr;
			hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, this->cb_buffer.GetAddressOf());
		};
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

}