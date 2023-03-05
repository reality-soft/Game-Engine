#pragma once
#include "stdafx.h"

namespace reality
{
	struct Vertex
	{
		XMFLOAT3   p;
		XMFLOAT3   n;
		XMFLOAT4   c;
		XMFLOAT2   t;
	};

	struct LevelVertex
	{
		XMFLOAT3   p;
		XMFLOAT3   n;
		XMFLOAT4   c;
		XMFLOAT2   t;
		XMFLOAT2   t_layer;
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
		XMFLOAT3 GetMaxXYZ()
		{
			XMFLOAT3 max_xyz = {0, 0, 0};
			for (auto mesh : meshes)
			{
				for (auto vertex : mesh.vertices)
				{
					max_xyz.x = max(max_xyz.x, vertex.p.x);
					max_xyz.y = max(max_xyz.y, vertex.p.y);
					max_xyz.z = max(max_xyz.z, vertex.p.z);
				}
			}
			return max_xyz;
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
			XMVECTOR camera_position;
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

	struct CbSkySphere
	{
		CbSkySphere()
		{
			data.time      = { 360, 360, 360, 360 };
			data.sky_color = { 1.0f,  1.0f,   1.0f,  1.0f };
		}
		struct Data
		{
			XMFLOAT4 time;
			XMFLOAT4 sky_color;
			XMFLOAT4 strength;
		} data;

		ComPtr<ID3D11Buffer> buffer;
	};

	struct InstanceData
	{
		InstanceData()
		{
			col_transform.identity();
			index = 0;

			S = { 1 ,1, 1 };
			R = { 0 ,0, 0 };
			T = { 0 ,0, 0 };
		}
		string GetName()
		{			
			return obj_name + "_" + to_string(index);
		}
		string obj_name;
		UINT index;

		XMFLOAT3 S;
		XMFLOAT3 R;
		XMFLOAT3 T;

		reactphysics3d::Transform col_transform;
		reactphysics3d::BoxShape* box_shape = nullptr;
		reactphysics3d::Collider* box_collider = nullptr;
		reactphysics3d::CollisionBody* collision_body = nullptr;
	};

	struct CbInstance
	{
		struct Data
		{
			XMMATRIX instance_transform[128];
		}data;

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

	// Effect
	

	enum E_Effect
	{
		NONE = 0,
		UV_SPRITE = 1,
		TEX_SPRITE = 2,
		EMITTER = 3,
	};

	enum E_Sprite
	{
		DEFAULT_SPRITE = 0,
		UV = 1,
		TEX = 2,
	};

	struct Sprite
	{
		int			max_frame = 5;
		E_Sprite	type = DEFAULT_SPRITE;
	};

	struct UVSprite : public Sprite
	{
		string						tex_id;
		vector<pair<POINT, POINT>>	uv_list;
		UVSprite()
		{
			tex_id = "";
			type = UV;
		}
	};

	struct TextureSprite : public Sprite
	{
		vector<string>	tex_id_list;
		TextureSprite()
		{
			type = TEX;
		}
	};

	struct CbSprite
	{
		__declspec(align(16)) struct Data
		{
			// 0 : UV, 1 : Texture
			//int type;
			//int max_frame;
			//int uv_list_size;
			//int padding;
			XMINT4 value;
			//float start_u[255];
			//float start_v[255];
			//float end_u[255];
			//float end_v[255];
			XMFLOAT4 value2[255];
			XMMATRIX billboard_matrix;
		} data;
		ComPtr<ID3D11Buffer> buffer;
	};

	struct CbParticle
	{
		struct Data
		{
			// x : timer
			// y : fixed_timer
			XMFLOAT4 values;
			XMFLOAT4 color;
			XMMATRIX transform;
		} data;
		ComPtr<ID3D11Buffer> buffer;
	};

	struct Particle
	{
		bool		enable;

		float		timer;
		float		lifetime;
		float		frame_ratio;

		XMFLOAT4	color;

		XMFLOAT3	position;
		XMFLOAT3	rotation;
		XMFLOAT3	scale;

		XMFLOAT3	add_velocity;
		XMFLOAT3	add_size;
		float		add_rotation;

		XMFLOAT3	accelation;


		vector<Vertex>			vertex_list;
		ComPtr<ID3D11Buffer>	vertex_buffer;

		vector<DWORD>			index_list;
		ComPtr<ID3D11Buffer>	index_buffer;

		Particle()
		{
			enable = true;

			timer = 0.0f;
			lifetime = 0.0f;
			frame_ratio = 0.0f;

			color = { 1.0, 1.0f, 1.0f, 1.0f };

			position = { 0, 0, 0 };
			rotation = { 0, 0, 0 };
			scale = { 0, 0, 0 };

			add_velocity = { 0, 0, 0 };
			add_size = { 0, 0, 0 };
			add_rotation = 0;

			accelation = { 0, 0, 0 };

		}

		void CreateBuffer()
		{
			
		}
	};

	struct Emitter
	{
		float		timer;

		string		sprite_id;

		int			emit_per_second;

		XMFLOAT4	color;

		float		life_time[2];

		XMFLOAT3	initial_size[2];
		float		initial_rotation[2];
		XMFLOAT3	initial_position[2];

		XMFLOAT3	initial_velocity[2];

		XMFLOAT3	size_per_lifetime[2];
		float		rotation_per_lifetime[2];
		XMFLOAT3	accelation_per_lifetime[2];

		string		vs_id;
		string		ps_id;
		string		geo_id;

		vector<Particle> particles;

		Emitter()
		{
			timer = 0.0f;

			sprite_id = "";

			emit_per_second = 0;

			color = { 1.0, 1.0f, 1.0f, 1.0f };

			ZeroMemory(life_time, sizeof(float) * 2);

			ZeroMemory(initial_size, sizeof(XMFLOAT3) * 2);
			ZeroMemory(initial_rotation, sizeof(float) * 2);
			ZeroMemory(initial_position, sizeof(XMFLOAT3) * 2);

			ZeroMemory(initial_velocity, sizeof(XMFLOAT3) * 2);

			ZeroMemory(size_per_lifetime, sizeof(XMFLOAT3) * 2);
			ZeroMemory(rotation_per_lifetime, sizeof(float) * 2);
			ZeroMemory(accelation_per_lifetime, sizeof(XMFLOAT3) * 2);

			vs_id = "";
			ps_id = "";
			geo_id = "";
		}
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
}
