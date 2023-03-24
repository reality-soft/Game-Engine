#pragma once
#include "Material.h"
#include "Mesh.h"

namespace reality
{
	struct InstanceData
	{
		InstanceData(string obj_name, UINT index)
		{
			instance_id = obj_name + "_" + to_string(index);
			this->index = index;

			S = { 1 ,1, 1 };
			R = { 0 ,0, 0 };
			T = { 0 ,0, 0 };

			cdata.world_matrix = XMMatrixIdentity();
		}

		UINT index;
		string instance_id;
		XMFLOAT3 S;
		XMFLOAT3 R;
		XMFLOAT3 T;

		struct CData
		{
			XMMATRIX world_matrix;
		} cdata;
	};

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
		map<UINT, InstanceData*> instance_pool;
		string object_name;

	public:
		InstanceData* AddNewInstance(string name);
		vector<InstanceData::CData> GetCDataArray();
		InstanceData* SelectInstance(UINT index);
		void SetInstanceScale(UINT index, XMFLOAT3 S);
		void SetInstanceRotation(UINT index, XMFLOAT3 R);
		void SetInstanceTranslation(UINT index, XMFLOAT3 T);

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

