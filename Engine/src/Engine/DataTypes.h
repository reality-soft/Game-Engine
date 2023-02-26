#pragma once
#include "stdafx.h"

namespace KGCA41B
{
	struct Vertex
	{
		XMFLOAT3   p;
		XMFLOAT3   n;
		XMFLOAT4   c;
		XMFLOAT2   t;
	};

	struct SkinnedVertex
	{
		XMFLOAT3   p;
		XMFLOAT3   n;
		XMFLOAT4   c;
		XMFLOAT2   t;
		XMFLOAT4   i;
		XMFLOAT4   w;

		SkinnedVertex operator +=(const Vertex& vertex)
		{
			this->p = vertex.p;
			this->n = vertex.n;
			this->c = vertex.c;
			this->t = vertex.t;
			return *this;
		}
	};

	struct Skeleton
	{
		Skeleton() = default;
		Skeleton(const Skeleton& other)
		{
			bind_pose_matrices = other.bind_pose_matrices;
		}

		map<UINT, XMMATRIX> bind_pose_matrices;
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
		SkeletalMesh(const SkeletalMesh& other)
		{
			meshes.resize(other.meshes.size());
			meshes = other.meshes;

			skeleton = other.skeleton;
		}

		vector<SingleMesh<SkinnedVertex>> meshes;
		Skeleton skeleton;
	};

	struct StaticMesh
	{
		StaticMesh() = default;
		StaticMesh(const StaticMesh& other)
		{
			meshes.resize(other.meshes.size());
			meshes = other.meshes;
		}

		vector<SingleMesh<Vertex>> meshes;
	};

	struct CbTransform
	{
		CbTransform()
		{
			data.world_matrix = XMMatrixIdentity();
		}
		CbTransform(const CbTransform& other)
		{
			data = other.data;
			other.buffer.CopyTo(buffer.GetAddressOf());
		}
		struct Data
		{
			XMMATRIX world_matrix;
		} data;

		ComPtr<ID3D11Buffer> buffer;
	};

	struct CbViewProj
	{
		CbViewProj()
		{
			data.view_matrix = XMMatrixIdentity();
			data.projection_matrix = XMMatrixIdentity();
		}
		CbViewProj(const CbViewProj& other)
		{
			data = other.data;
			other.buffer.CopyTo(buffer.GetAddressOf());
		}
		struct Data
		{
			XMMATRIX view_matrix;
			XMMATRIX projection_matrix;
		} data;
		ComPtr<ID3D11Buffer> buffer;
	};

	struct CbSkeleton
	{
		CbSkeleton() = default;
		CbSkeleton(const CbSkeleton& other)
		{
			data = other.data;
			other.buffer.CopyTo(buffer.GetAddressOf());
		}
		struct Data
		{
			XMMATRIX  mat_skeleton[255];
		} data;
		ComPtr<ID3D11Buffer> buffer;
	};

	// Physics
	struct MouseRay
	{
		reactphysics3d::Vector3 start_point;
		reactphysics3d::Vector3 end_point;
	};

	// Sound

	enum SoundType
	{
		SFX,
		MUSIC,
	};

	struct Sound
	{
		string sound_filename;
		SoundType type;
		bool looping;
		FMOD::Channel* channel;
		FMOD::Sound* sound;
		UINT total_time;
		UINT current_time;
	public:
		Sound() : sound_filename(""), channel(nullptr), sound(nullptr), total_time(0), current_time(0) { }
		Sound(Sound& sound) = default;
	};

	struct SoundQueue
	{
		string		sound_filename;
		SoundType	sound_type;
		float		sound_volume;
		bool		is_looping;
	};

	enum class AxisType
	{
		FROWARD,
		RIGHT,
		UP,
		YAW,
		PITCH,
		ROLL,

		IDLE
	};

	enum class ActionType
	{
		JUMP,
		SIT,
		DASH,

		ATTACK,
		THROW,

		SKILL1,
		SKILL2,
		SKILL3,

		IDLE
	};

	// Effect
	enum E_Effect
	{
		NONE = 0,
		UV_SPRITE = 1,
		TEX_SPRITE = 2,
		PARTICLES = 3,
	};

	struct UVSpriteData
	{
		int max_frame = 10;
		int cur_frame = 1;
		string texture_id = "";
		vector<pair<POINT, POINT>> uv_list;
		string vs_id = "";
		string ps_id = "";
		char effect_name[255] = "";
	};

	struct TexSpriteData
	{
		int max_frame = 10;
		int cur_frame = 1;
		string texture_id = "";
		vector<string> tex_id_list;
		string vs_id = "";
		string ps_id = "";
		char effect_name[255] = "";
	};

}