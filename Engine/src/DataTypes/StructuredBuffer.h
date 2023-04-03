#pragma once

namespace reality
{
	class StructuredSRV
	{
	public:
		virtual bool Create(void* p_data);

	public:
		size_t byte_stride = 0;
		size_t byte_width = 0;

		ComPtr<ID3D11Buffer> buffer;
		ComPtr<ID3D11ShaderResourceView> srv;
	};

	class StructuredUAV
	{
	public:
		virtual bool Create(void* p_data);

	public:
		size_t byte_stride = 0;
		size_t byte_width = 0;

		ComPtr<ID3D11Buffer> buffer;
		ComPtr<ID3D11UnorderedAccessView> uav;
	};

	class SbTriangleCollision : public StructuredSRV
	{
	public:
		struct Data
		{
			UINT index;
			XMFLOAT3 normal;
			XMFLOAT3 vertex0, vertex1, vertex2;
		};

	public:
		void SetElementArraySize(UINT size);
		vector<SbTriangleCollision::Data> elements;
	};

	class SbCapsuleCollision : public StructuredSRV
	{
	public:
		struct Data
		{
			float radius;
			XMFLOAT3 point_a;
			XMFLOAT3 point_b;
			unsigned short triangle_index[1000];
		};

	public:
		void SetElementArraySize(UINT size);
		vector<SbCapsuleCollision::Data> elements;
	};

	class SbCollisionResult : public StructuredUAV
	{
	public:
		struct Data
		{
			int result;
			XMFLOAT3 position;
		};

	public:
		void SetElementArraySize(UINT size);
		vector<SbCollisionResult::Data> elements;
	};
}