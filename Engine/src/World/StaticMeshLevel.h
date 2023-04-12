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
		bool Create(string mesh_id, string vs_id, string collision_ltmesh);
		void Update();
		void Render();
		void Destroy();

		void SetShadowMap(ID3D11ShaderResourceView* shadow_map, CbShadowMap* cb_shaodow);
		void RenderCollisionMesh();
		StaticMesh* GetLevelMesh();
		VertexShader* GetVertexShader();
	public:
		CbTransform level_transform;
		vector<TriangleShape> level_triangles;

	private:
		shared_ptr<StaticMesh> level_mesh;
		shared_ptr<StaticMesh> collision_mesh;
		shared_ptr<VertexShader> vertex_shader;


		bool SetMaterialToMesh(string mesh_name, string material_id);
		bool SetRandomMaterialToMesh(string mesh_name, string keyward);

	public:
		void ImportGuideLines(string mapdat_file, GuideLine::GuideType guide_type);
		vector<GuideLine> GetGuideLines(GuideLine::GuideType guide_type);

	private:
		ID3D11RasterizerState* clipping_rs = nullptr;
		map<string, shared_ptr<Material>> mesh_material_map;
		vector<GuideLine> guide_lines;
	};
}

