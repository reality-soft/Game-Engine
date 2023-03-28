#pragma once
#include "Vertex.h"

namespace reality
{
	struct Skeleton
	{
		Skeleton() = default;
		Skeleton(const Skeleton& other);

		map<UINT, XMMATRIX> bind_pose_matrices;
		map<string, UINT> skeleton_id_map;
	};

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
}

