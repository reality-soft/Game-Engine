#pragma once
#include "DataTypes.h"
#include "DllMacro.h"
#include "Shader.h"
#include "Texture.h"

#include <variant>
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

		void Update();
		void Render();

	private:
		void GenVertexNormal();
		XMFLOAT3 GetNormal(UINT i0, UINT i1, UINT i2);
		bool CreateBuffers();

	private:
		SingleMesh<Vertex> level_mesh_;

		string vs_id_;
		string ps_id_;
		vector<string> texture_id;

		CbTransform level_transform_;
		CbLight level_light_;

		UINT num_row_vertex_;
		UINT num_col_vertex_;

		float cell_distance_;
		float uv_scale_;
		float max_height_;
		vector<float> height_list_;

	private:
		ID3D11Device* device_;
		ID3D11DeviceContext* device_context_;
	};
}

