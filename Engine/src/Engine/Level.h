#pragma once
#include "DataTypes.h"
#include "DllMacro.h"
#include "Components.h"

#define LerpByTan(start, end, tan) (start - (start * tan) + (end * tan))


namespace KGCA41B
{
	struct CbHitCircle
	{
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
		Level() {};
		~Level() {}

	public:
		bool CreateLevel(UINT num_row, UINT num_col, int cell_distance, int uv_scale);
		bool CreateHeightField(float min_height, float max_height);

		void Update();
		void Render();

		XMINT2 GetWorldSize();

	public: // Editings
		XMVECTOR LevelPicking(const MouseRay& mouse_ray, float circle_radius);
		void LevelEdit(const MouseRay& mouse_ray, float circle_radius);
		void Regenerate(UINT num_row, UINT num_col, int cell_distance, int uv_scale);
		void ResetHeightField();
		float sculpting_brush_ = 100.0f;

		vector<Vertex> GetLevelVertex() { return level_mesh_.vertices; }
		vector<UINT> GetLevelIndex() { return level_mesh_.indices; }

	private:
		void GenVertexNormal();
		float GetHeightAt(float x, float y);
		void GetHeightList();

		XMFLOAT3 GetNormal(UINT i0, UINT i1, UINT i2);
		bool CreateBuffers();
		bool CreateEditBuffer(ID3D11Buffer** _buffer);


	public:
		bool edit_mode = false;

		string vs_id_;
		string ps_id_;
		string gs_id_;
		vector<string> texture_id;

	private:
		SingleMesh<Vertex> level_mesh_;
		ComPtr<ID3D11Buffer> so_buffer_;

		CbTransform level_transform_;
		CbLight level_light_;
		CbHitCircle hit_circle_;
		CbEditOption edit_option_;

		UINT num_row_vertex_;
		UINT num_col_vertex_;

		int cell_distance_;
		int uv_scale_;
		vector<float> height_list_;

	private:
		reactphysics3d::HeightFieldShape* height_field_shape_;
		reactphysics3d::Collider* height_field_collider_;
		reactphysics3d::CollisionBody* height_field_body_;
	};
}