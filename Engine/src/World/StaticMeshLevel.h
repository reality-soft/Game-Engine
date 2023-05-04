#pragma once
#include "Components.h"
#include "Mesh.h"
#include "ConstantBuffer.h"
#include "GuideLine.h"

namespace reality
{
	class DLL_API StaticMeshLevel
	{
	public:
		StaticMeshLevel();
		~StaticMeshLevel();

	public:
		bool Create(string mesh_id, string vs_id);
		void Update();
		void Render();
		void Destroy();

		void RenderCollisionMesh();
		StaticMesh* GetLevelMesh();
		VertexShader* GetVertexShader();
	public:
		CbTransform level_transform;
		vector<TriangleShape> level_triangles;

	private:
		shared_ptr<StaticMesh> level_mesh;
		shared_ptr<VertexShader> vertex_shader;
		ID3D11RasterizerState* clipping_rs = nullptr;
	};
}

