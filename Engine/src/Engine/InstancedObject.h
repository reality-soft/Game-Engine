#pragma once
#include "DllMacro.h"
#include "DataTypes.h"
#include "Material.h"

namespace reality
{
	class DLL_API InstancedObject
	{
	public:
		InstancedObject();
		~InstancedObject();

	public:
		bool Init(string mesh_id, string vs_id, string mat_id);
		void Frame();
		void Render();
		void Release();

		InstanceData* selected_instance = nullptr;
		map<string, InstanceData*> instance_pool;
		string object_name;

	public:
		InstanceData* AddNewInstance(string name);
		vector<InstanceData::CData> GetCDataArray();
		void SetInstanceScale(string name, XMFLOAT3 S);
		void SetInstanceRotation(string name, XMFLOAT3 R);
		void SetInstanceTranslation(string name, XMFLOAT3 T);

	private:
		bool UpdateInstance();
		bool CreateInstanceBuffer();


	private:
		shared_ptr<StaticMesh> static_mesh;
		shared_ptr<VertexShader> vertex_shader;
		shared_ptr<Material> material;

		ComPtr<ID3D11Buffer> inst_buffer;
		ComPtr<ID3D11ShaderResourceView> inst_srv;

	public:
		StaticMesh* GetStaticMesh() { return static_mesh.get(); }
		VertexShader* GetVertexShader() { return vertex_shader.get(); }

		ID3D11Buffer* GetInstanceBuffer() { return inst_buffer.Get(); }
		ID3D11ShaderResourceView* GetInstanceSRV() { return inst_srv.Get(); }

	};
}

