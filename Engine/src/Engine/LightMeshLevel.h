#pragma once
#include "Components.h"
#include "DllMacro.h"
#include "DataTypes.h"
#include "ResourceMgr.h"
#include "InstancedObject.h"

namespace reality
{
	class DLL_API LightMeshLevel
	{
	public:
		LightMeshLevel();
		~LightMeshLevel();

	public:
		bool Create(string mesh_id, string vs_id, string gs_id);
		void Update();
		void Render();
		void Destroy();


	public:
		CbTransform level_transform;
		vector<TriangleShape> level_triangles;

	private:
		shared_ptr<LightMesh> level_mesh;
		shared_ptr<VertexShader> vertex_shader;
		shared_ptr<GeometryShader> geometry_shader;

		//reactphysics3d::TriangleMesh* shape_mesh = nullptr;
		//reactphysics3d::ConcaveMeshShape* level_shape = nullptr;
		//reactphysics3d::Collider* level_collider = nullptr;
		//reactphysics3d::CollisionBody* level_body = nullptr;
	};
}

