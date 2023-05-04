#pragma once
#include "stdafx.h"

namespace reality
{
	struct SocketTransform {
		XMMATRIX	owner_local = XMMatrixIdentity();
		XMMATRIX	local_offset = XMMatrixIdentity();
		XMMATRIX	animation_matrix = XMMatrixIdentity();
	};

	struct CbTransform
	{
		CbTransform()
		{
			data.transform_matrix = XMMatrixIdentity();
		}
		CbTransform(const CbTransform& other)
		{
			data = other.data;
			other.buffer.CopyTo(buffer.GetAddressOf());
		}
		struct Data
		{
			XMMATRIX transform_matrix;
		} data;

		ComPtr<ID3D11Buffer> buffer;
	};

	struct CbStaticMesh
	{
		CbStaticMesh() = default;
		CbStaticMesh(const CbStaticMesh& other)
		{
			data = other.data;
			other.buffer.CopyTo(buffer.GetAddressOf());
		}
		struct Data
		{
			XMMATRIX transform_matrix;
			XMMATRIX socket_matrix;
		} data;
		ComPtr<ID3D11Buffer> buffer;
	};


	struct CbSkeletalMesh
	{
		CbSkeletalMesh() = default;
		CbSkeletalMesh(const CbSkeletalMesh& other)
		{
			data = other.data;
			other.buffer.CopyTo(buffer.GetAddressOf());
		}
		struct Data
		{
			XMMATRIX    transform_matrix;
			XMMATRIX	animation[128];
		} data;
		ComPtr<ID3D11Buffer> buffer;
	};

	struct CbCameraInfo
	{
		CbCameraInfo()
		{
			data.view_proj_matrix = XMMatrixIdentity();
			data.camera_translation = XMMatrixIdentity();
			data.camera_position = XMVectorZero();
			data.camera_look = XMVectorZero();
		}
		CbCameraInfo(const CbCameraInfo& other)
		{
			data = other.data;
			other.buffer.CopyTo(buffer.GetAddressOf());
		}
		struct Data
		{
			XMMATRIX view_proj_matrix;
			XMMATRIX camera_translation;
			XMVECTOR camera_position;
			XMVECTOR camera_look;
		} data;
		ComPtr<ID3D11Buffer> buffer;
	};

	struct CbCameraEffect
	{
		CbCameraEffect()
		{
			data.main_billboard = XMMatrixIdentity();
		}
		CbCameraEffect(const CbCameraEffect& other)
		{
			data = other.data;
			other.buffer.CopyTo(buffer.GetAddressOf());
		}
		struct Data
		{
			XMMATRIX view_matrix;
			XMMATRIX projection_matrix;
			XMMATRIX main_billboard;
			XMMATRIX x_billboard;
			XMMATRIX y_billboard;
		} data;
		ComPtr<ID3D11Buffer> buffer;
	};

	struct CbSkySphere
	{
		CbSkySphere()
		{
			data.sky_color = { 0, 0, 0, 0 };
			data.gradation = { 0, 0, 0, 0 };
		}
		struct Data
		{
			XMFLOAT4 sky_color;
			XMFLOAT4 gradation;
		} data;

		ComPtr<ID3D11Buffer> buffer;
	};

	struct CbFog
	{
		CbFog()
		{
			data.fog_color = { 1, 1, 1, 1 };
			data.fog_start = { 0, 0, 0 };
			data.distance = 5000;
		}
		struct Data
		{
			XMFLOAT4 fog_color;
			XMFLOAT3 fog_start;
			float distance;
		} data;

		ComPtr<ID3D11Buffer> buffer;
	};

	struct CbGlobalLight
	{
		CbGlobalLight()
		{
			data.position = XMFLOAT4(5000, 5000, -5000, 0);
			data.light_color = XMFLOAT4(1, 1, 1, 1);

			data.direction = XMFLOAT3((-1.0f * XMVector3Normalize(XMLoadFloat4(&data.position))).m128_f32);
			data.specular_strength = 1.0f;
			
			data.ambient_up    = XMLoadFloat3(&data.direction) * 0.8;
			data.ambient_down  = XMLoadFloat3(&data.direction) * 0.2f;
			data.ambient_range = Vector3Length(data.ambient_up - data.ambient_down);
		}
		CbGlobalLight(const CbGlobalLight& other)
		{
			data = other.data;
			other.buffer.CopyTo(buffer.GetAddressOf());
		}
		struct Data
		{
			XMFLOAT4 position;
			XMFLOAT4 light_color;

			XMFLOAT3 direction;
			float specular_strength;

			XMVECTOR ambient_up;
			XMVECTOR ambient_down;
			float ambient_range;
		} data;

		ComPtr<ID3D11Buffer> buffer;
	};

	struct CbPointLight
	{
		struct Data
		{
			XMFLOAT4	light_color;

			XMFLOAT3	position;
			float		range;
			XMFLOAT3	attenuation;
			float		specular;
		} data[64];

		ComPtr<ID3D11Buffer> buffer;

		CbPointLight()
		{
			ZeroMemory(data, sizeof(Data) * 64);
		}
		
	};

	struct CbSpotLight
	{
		struct Data
		{
			XMFLOAT4	light_color;

			XMFLOAT3	position;
			float		range;
			XMFLOAT3	attenuation;
			float		specular;
			XMFLOAT3	direction;
			float		spot;
		} data[64];

		ComPtr<ID3D11Buffer> buffer;

		CbSpotLight()
		{
			ZeroMemory(data, sizeof(Data) * 64);
		}
	};

}