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
		// Import
		bool ImportFromFile(string filepath);

	public:
		// Create
		bool CreateLevel(UINT _max_lod, UINT _cell_scale, UINT _uv_scale, XMINT2 _row_col_blocks);
		void Update();
		void Render(bool culling);

	public:
		// Getter
		float GetHeightAt(float x, float y);
		XMVECTOR GetNormalAt(float x, float y);
		XMINT2 GetWorldSize();
		XMINT2 GetBlocks();
		UINT MaxLod();
		vector<Vertex> GetLevelVertex() { return level_mesh_.vertices; }
		vector<UINT> GetLevelIndex() { return level_mesh_.indices; }
		reactphysics3d::CollisionBody* GetCollisionBody() { return height_field_body_; }

	public:
		string vs_id_;
		string ps_id_;
		vector<string> texture_id;

	protected:
		void GenVertexNormal();
		void GetHeightList();
		XMFLOAT2 GetMinMaxHeight();
		XMFLOAT3 GetNormal(UINT i0, UINT i1, UINT i2);
		bool CreateBuffers();
		bool CreateHeightField(float min_height, float max_height);

	protected:
		SingleMesh<Vertex> level_mesh_;
		vector<float> height_list_;
		ComPtr<ID3D11SamplerState> mip_map_sample;

		UINT num_row_vertex_;
		UINT num_col_vertex_;

		UINT max_lod;
		float cell_distance_;
		UINT cell_scale;
		XMINT2 row_col_blocks;
		float uv_scale_;		 

	protected:
		reactphysics3d::HeightFieldShape* height_field_shape_ = nullptr;
		reactphysics3d::Collider* height_field_collider_ = nullptr;
		reactphysics3d::CollisionBody* height_field_body_ = nullptr;
	};
}