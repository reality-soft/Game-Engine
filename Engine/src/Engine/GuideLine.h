#pragma once
#include "DllMacro.h"
#include "DataTypes.h"
#include "Shader.h"

namespace reality
{
	class DLL_API GuideLine
	{
	public:
		enum GuideType
		{
			eBlocking,
			eNpcTrack,
		} guide_type;

	public:
		bool Create(string mesh_id, string vs_id, string ps_id);
		void AddNode(XMVECTOR node_pos);
		void DrawLines(XMFLOAT4 color);

		CbTransform node_transform;

	private:
		list<XMVECTOR> line_nodes;
		vector<XMVECTOR> line_vectors;

		SingleMesh<Vertex> line_mesh;
		SingleMesh<Vertex> node_mesh;

		shared_ptr<VertexShader> vertex_shader;
		shared_ptr<PixelShader> pixel_shader;
	};
}


