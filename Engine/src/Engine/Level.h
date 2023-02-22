#pragma once
#include "DataTypes.h"
#include "DllMacro.h"
#include "Components.h"

#define LerpByTan(start, end, tan) (start - (start * tan) + (end * tan))


namespace KGCA41B
{
	struct CbHitCircle
	{
		CbHitCircle() = default;
		CbHitCircle(const CbHitCircle& other)
		{
			data = other.data;
			other.buffer.CopyTo(buffer.GetAddressOf());
		}
		struct Data
		{
			bool is_hit = false;
			float circle_radius = 0.0f;
			XMVECTOR hitpoint = { 0, 0, 0, 0 };
		} data;

		ComPtr<ID3D11Buffer> buffer;
	};

	struct CbEditOption
	{
		CbEditOption() = default;
		CbEditOption(const CbEditOption& other)
		{
			data = other.data;
			other.buffer.CopyTo(buffer.GetAddressOf());
		}
		struct Data
		{
			XMINT4 altitude = {0, 0, 0, 0};

		} data;
		ComPtr<ID3D11Buffer> buffer;
	};
	struct StreamVertex
	{
		XMFLOAT4 p;
		XMFLOAT3 o;
		XMFLOAT3 n;
		XMFLOAT4 c;
		XMFLOAT2 t;
	};

	class DLL_API Level
	{
	public:
		Level() = default;
		~Level() = default;

	public:
		bool ImportFromFile(string filepath);

	public:
		bool CreateLevel(UINT num_row, UINT num_col, int cell_distance, int uv_scale);
		bool CreateHeightField(float min_height, float max_height);

		void Update();
		void Render(bool culling);

		XMINT2 GetWorldSize();
		vector<Vertex> GetLevelVertex() { return level_mesh_.vertices; }
		vector<UINT> GetLevelIndex() { return level_mesh_.indices; }

	protected:
		void GenVertexNormal();
		float GetHeightAt(float x, float y);
		void GetHeightList();

		XMFLOAT2 GetMinMaxHeight();
		XMFLOAT3 GetNormal(UINT i0, UINT i1, UINT i2);
		bool CreateBuffers();

	public:
		string vs_id_;
		string ps_id_;
		vector<string> texture_id;

	protected:
		SingleMesh<Vertex> level_mesh_;
		vector<float> height_list_;
		ComPtr<ID3D11SamplerState> mip_map_sample;

		CbTransform level_transform_;
		CbLight level_light_;

		UINT num_row_vertex_;
		UINT num_col_vertex_;

		int cell_distance_;
		int uv_scale_;


	protected:
		reactphysics3d::HeightFieldShape* height_field_shape_ = nullptr;
		reactphysics3d::Collider* height_field_collider_ = nullptr;
		reactphysics3d::CollisionBody* height_field_body_ = nullptr;
	};
}