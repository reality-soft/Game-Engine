#pragma once
#include "DllMacro.h"
#include "DataTypes.h"
#include "Shader.h"

namespace KGCA41B
{
	class DLL_API InstancedObject
	{
	public:
		InstancedObject();
		~InstancedObject();

	public:
		void Init(string mesh_id, string vs_id);
		void Frame();
		void Render();
		void Release();

		vector<InstanceData> instance_list;
		InstanceData* selected_instance = nullptr;
		string object_name;

	public:
		void AddNewInstance();
		InstanceData* FindAndSelectWithCollision(reactphysics3d::CollisionBody* col_body);
		XMMATRIX TransformS(XMFLOAT3& sacling);
		XMMATRIX TransformR(XMFLOAT3& roation);
		XMMATRIX TransformT(XMFLOAT3& position);

		void UpdateInstance();

	private:
		void CreateInstanceBuffer();
		shared_ptr<StaticMesh> static_mesh;
		shared_ptr<VertexShader> vs;
		CbInstance instancing;

	private:
	};
}

