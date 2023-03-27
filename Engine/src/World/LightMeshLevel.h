#pragma once
#include "Components.h"
#include "Mesh.h"
#include "ConstantBuffer.h"
#include "GuideLine.h"

namespace reality
{
	class DLL_API LightMeshLevel
	{
	public:
		LightMeshLevel();
		~LightMeshLevel();

	public:
		bool Create(string mesh_id, string vs_id, string gs_id, string collision_ltmesh);
		void Update();
		void Render();
		void Destroy();

		void RenderCollisionMesh();
	public:
		CbTransform level_transform;
		vector<TriangleShape> level_triangles;

	private:
		shared_ptr<LightMesh> level_mesh;
		shared_ptr<LightMesh> collision_mesh;
		shared_ptr<VertexShader> vertex_shader;
		shared_ptr<GeometryShader> geometry_shader;

		bool SetMaterialToMesh(string mesh_name, string material_id);
		bool SetRandomMaterialToMesh(string mesh_name, string keyward);

	public:
		void ImportGuideLines(string mapdat_file, GuideLine::GuideType guide_type);
		vector<GuideLine> GetGuideLines(GuideLine::GuideType guide_type);

	private:
		map<string, shared_ptr<Material>> mesh_material_map;
		vector<GuideLine> guide_lines;
		bool view_collision = true;

	};
}

