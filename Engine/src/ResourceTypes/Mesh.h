#pragma once
#include "DX11App.h"
#include "Vertex.h"
#include "Skeleton.h"

namespace reality
{
	template <typename VertexType>
	struct SingleMesh
	{
		SingleMesh() = default;
		SingleMesh(const SingleMesh<VertexType>& other)
		{
			mesh_name = other.mesh_name;

			vertices.resize(other.vertices.size());
			vertices = other.vertices;

			indices.resize(other.indices.size());
			indices = other.indices;

			other.vertex_buffer.CopyTo(vertex_buffer.GetAddressOf());
			other.index_buffer.CopyTo(index_buffer.GetAddressOf());
		}
		~SingleMesh()
		{

			vertices.clear();
			indices.clear();
			vertex_buffer.ReleaseAndGetAddressOf();
			index_buffer.ReleaseAndGetAddressOf();
		}

		string mesh_name;

		vector<VertexType> vertices;
		ComPtr<ID3D11Buffer> vertex_buffer;

		vector<UINT> indices;
		ComPtr<ID3D11Buffer> index_buffer;
	};

	struct SkeletalMesh
	{
		SkeletalMesh() = default;
		SkeletalMesh(const SkeletalMesh& other);

		vector<SingleMesh<SkinnedVertex>> meshes;
		Skeleton skeleton;
	};

	struct StaticMesh
	{
		StaticMesh() = default;
		StaticMesh(const StaticMesh& other);

		vector<SingleMesh<Vertex>> meshes;
	};

	template<typename MeshType>
	static XMFLOAT3 GetMaxVertex(const MeshType& mesh)
	{
		XMFLOAT3 max_xyz = { 0, 0, 0 };
		for (auto& single_mesh : mesh.meshes)
		{
			for (auto vertex : single_mesh.vertices)
			{
				max_xyz.x = max(max_xyz.x, vertex.p.x);
				max_xyz.y = max(max_xyz.y, vertex.p.y);
				max_xyz.z = max(max_xyz.z, vertex.p.z);
			}
		}
		return max_xyz;
	}

	static bool CreateVertexBuffer(SingleMesh<Vertex>& single_mesh)
	{
		single_mesh.vertex_buffer.ReleaseAndGetAddressOf();
		single_mesh.vertex_buffer = nullptr;

		HRESULT hr;

		// VertexBuffer
		D3D11_BUFFER_DESC desc;
		D3D11_SUBRESOURCE_DATA subdata;

		ZeroMemory(&desc, sizeof(desc));
		ZeroMemory(&subdata, sizeof(subdata));

		desc.ByteWidth = sizeof(Vertex) * single_mesh.vertices.size();
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		subdata.pSysMem = single_mesh.vertices.data();

		hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, single_mesh.vertex_buffer.GetAddressOf());
		if (FAILED(hr))
			return false;

		return true;
	}
}

