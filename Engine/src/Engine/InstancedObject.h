#pragma once
#include "DllMacro.h"
#include "DataTypes.h"
#include "Shader.h"

namespace reality
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

		InstanceData* selected_instance = nullptr;
		string object_name;
		string mesh_id_;
		string vs_id_;

	public:
		void AddNewInstance();

	private:
		bool UpdateInstance();
		bool CreateInstanceBuffer();

	private:
		map<string, InstanceData*> instance_pool;

	public:
		vector<InstanceData::CData> GetCDataArray();

	private:
		shared_ptr<StaticMesh> static_mesh;
		shared_ptr<VertexShader> vertex_shader;

		ComPtr<ID3D11Buffer> inst_buffer;
		ComPtr<ID3D11ShaderResourceView> inst_srv;

	public:
		StaticMesh* GetStaticMesh() { return static_mesh.get(); }
		VertexShader* GetVertexShader() { return vertex_shader.get(); }

		ID3D11Buffer* GetInstanceBuffer() { return inst_buffer.Get(); }
		ID3D11ShaderResourceView* GetInstanceSRV() { return inst_srv.Get(); }

	};
}

