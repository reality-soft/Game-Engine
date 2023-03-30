#pragma once
#include "stdafx.h"

namespace reality
{
	struct CbTransform
	{
		CbTransform()
		{
			data.world_matrix = XMMatrixIdentity();
		}
		CbTransform(const CbTransform& other)
		{
			data = other.data;
			other.buffer.CopyTo(buffer.GetAddressOf());
		}
		struct Data
		{
			XMMATRIX world_matrix;
		} data;

		ComPtr<ID3D11Buffer> buffer;
	};

	struct CbCameraInfo
	{
		CbCameraInfo()
		{
			data.view_matrix = XMMatrixIdentity();
			data.projection_matrix = XMMatrixIdentity();
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
			XMMATRIX view_matrix;
			XMMATRIX projection_matrix;
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

	struct CbSkeleton
	{
		CbSkeleton() = default;
		CbSkeleton(const CbSkeleton& other)
		{
			data = other.data;
			other.buffer.CopyTo(buffer.GetAddressOf());
		}
		struct Data
		{
			XMMATRIX  mat_skeleton[255];
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
			data.position = XMVectorSet(5000, 5000, -5000, 0);
			data.direction = -XMVector3Normalize(data.position);
			data.ambient_up    = data.direction * 0.8;
			data.ambient_down  = data.direction * 0.2f;
			data.ambient_range = Vector3Length(data.ambient_up - data.ambient_down);
		}
		CbGlobalLight(const CbGlobalLight& other)
		{
			data = other.data;
			other.buffer.CopyTo(buffer.GetAddressOf());
		}
		struct Data
		{
			XMVECTOR position;
			XMVECTOR direction;
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
			XMFLOAT3	diffuse;
			float		pad1;
			XMFLOAT3	specular;
			float		pad2;
			XMFLOAT3	ambient;
			float		pad3;

			XMFLOAT3	position;
			float		range;
			XMFLOAT3	attenuation;
			float		pad4;
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
			XMFLOAT3	diffuse;
			float		pad1;
			XMFLOAT3	specular;
			float		pad2;
			XMFLOAT3	ambient;
			float		pad3;

			XMFLOAT3	position;
			float		range;
			XMFLOAT3	attenuation;
			float		pad4; 
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