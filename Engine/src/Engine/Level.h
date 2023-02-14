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
			XMFLOAT4 circle_color = { 0, 0, 0, 0 };
		} data;

		ComPtr<ID3D11Buffer> buffer;
	};

	class DLL_API Level
	{
	public:
		Level();
		~Level() {}

	public:
		bool CreateLevel(UINT num_row, UINT num_col, int cell_distance, int uv_scale);
		bool CreateHeightField(float min_height, float max_height);

		void Update();
		void Render();
		XMVECTOR LevelPicking(const MouseRay& mouse_ray, float circle_radius, XMFLOAT4 circle_color);

	private:
		void GenVertexNormal();
		float GetHeightAt(float x, float y);
		void GetHeightList();

		XMFLOAT3 GetNormal(UINT i0, UINT i1, UINT i2);
		bool CreateBuffers();
		
	public:
		string vs_id_;
		string ps_id_;
		vector<string> texture_id;

	private:
		SingleMesh<Vertex> level_mesh_;

		CbTransform level_transform_;
		CbLight level_light_;
		CbHitCircle hit_circle_;

		UINT num_row_vertex_;
		UINT num_col_vertex_;

		int cell_distance_;
		int uv_scale_;
		float max_height_;
		vector<float> height_list_;

	private:
		reactphysics3d::HeightFieldShape* height_field_shape_;
		reactphysics3d::Collider* height_field_collider_;
		reactphysics3d::CollisionBody* height_field_body_;

	private:
		ID3D11Device* device_;
		ID3D11DeviceContext* device_context_;
	};
}

