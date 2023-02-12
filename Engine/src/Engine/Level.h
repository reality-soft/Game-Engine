#pragma once
#include "DataTypes.h"
#include "DllMacro.h"
#include "Components.h"


namespace KGCA41B
{
	class DLL_API Level
	{
	public:
		Level();
		~Level() {}

	public:
		bool CreateLevel(UINT num_row, UINT num_col, float cell_distance, float uv_scale);
		bool CreateHeightField(float min_height, float max_height);

		void Update();
		void Render();
		XMVECTOR LevelPicking(MouseRay* mouse_ray);

	private:
		void GenVertexNormal();
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

		UINT num_row_vertex_;
		UINT num_col_vertex_;

		float cell_distance_;
		float uv_scale_;
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

