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

	struct LightVertex
	{
		XMFLOAT3 p;
		XMFLOAT2 t;
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

	struct EffectVertex
	{
		XMFLOAT3   p;
		XMFLOAT4   c;
		XMFLOAT2   t;
	};

	struct UIVertex
	{
		XMFLOAT2 p;
		XMFLOAT2 t;
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

	struct LightMesh
	{
		LightMesh() = default;
		LightMesh(const LightMesh& other)
		{
			meshes.resize(other.meshes.size());
			meshes = other.meshes;
		}

		vector<SingleMesh<LightVertex>> meshes;
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

	struct CbCameraEffect
	{
		CbCameraEffect()
		{
			data.main_billboard = XMMatrixIdentity();
		}
		CbCameraEffect(const CbCameraEffect& other)
		{
			data = other.data;
			other.buffer.CopyTo(buffer.GetAddressOf());
		}
		struct Data
		{
			XMMATRIX view_matrix;
			XMMATRIX projection_matrix;
			XMMATRIX main_billboard;
			XMMATRIX x_billboard;
			XMMATRIX y_billboard;
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
			data.time      = { 240, 480, 240, 480 };
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
		InstanceData(string obj_name, UINT index)
		{
			instance_id = obj_name + "_" + to_string(index);

			S = { 1 ,1, 1 };
			R = { 0 ,0, 0 };
			T = { 0 ,0, 0 };

			collision_transform.identity();

			cdata.world_matrix = XMMatrixIdentity();
		}

		string instance_id;
		XMFLOAT3 S;
		XMFLOAT3 R;
		XMFLOAT3 T;

		reactphysics3d::Transform collision_transform;

		struct CData
		{
			XMMATRIX world_matrix;
		} cdata;
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

#define EFFECT_TIMELINE_SIZE 101

	enum E_EffectType
	{
		NONE			= 0,
		UV_SPRITE		= 1,
		TEX_SPRITE		= 2,
		EMITTER			= 3,
		EFFECT			= 5,
	};

	enum E_EmitterAttributeType
	{
		INITIAL_SET			= 0,
		ADD_PER_LIFETIME	= 1,
		SET_PER_LIFETIME	= 2,
	};

	enum E_EffectBS
	{
		DEFAULT_BS			= 0,
		NO_BLEND			= 1,
		ALPHA_BLEND			= 2,
		DUALSOURCE_BLEND	= 3,
		HIGHER_RGB			= 4,
	};

	enum E_EffectDS
	{
		DEFAULT_NONE		= 0,
		DEPTH_COMP_NOWRITE	= 1,
		DEPTH_COMP_WRITE	= 2,
	};

	enum E_EmitType
	{
		PER_SECOND	= 0,
		ONCE		= 1,
		AFTER_TIME	= 2,
	};

	struct Sprite
	{
		E_EffectType	type = NONE;
	};

	struct UVSprite : public Sprite
	{
		string						tex_id;
		vector<pair<POINT, POINT>>	uv_list;
		UVSprite()
		{
			tex_id = "";
			type = UV_SPRITE;
		}
	};

	struct TextureSprite : public Sprite
	{
		vector<string>	tex_id_list;
		TextureSprite()
		{
			type = TEX_SPRITE;
		}
	};

	struct CbEffect
	{
		__declspec(align(16)) struct Data
		{
			XMMATRIX world;
		} data;
		ComPtr<ID3D11Buffer> buffer;
	};

	struct CbEmitter
	{
		__declspec(align(16)) struct Data
		{
			// 0 : UV, 1 : Texture
			//int type;
			//int padding;
			//int uv_list_size;
			//int padding;
			XMINT4 value;
			//float start_u[255];
			//float start_v[255];
			//float end_u[255];
			//float end_v[255];
			XMFLOAT4 value2[255];
			XMMATRIX world;
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
		XMFLOAT3	velocity;
		XMFLOAT3	rotation;
		XMFLOAT3	scale;

		XMFLOAT3	add_size;
		XMFLOAT3		add_rotation;
		XMFLOAT3	accelation;

		Particle()
		{
			enable = true;

			timer = 0.0f;
			lifetime = 0.0f;
			frame_ratio = 0.0f;

			color = { 1.0, 1.0f, 1.0f, 1.0f };

			position	= { 0, 0, 0 };
			rotation	= { 0, 0, 0 };
			scale		= { 0, 0, 0 };
			velocity	= { 0, 0, 0 };

			add_size		= { 0, 0, 0 };
			add_rotation	= { 0, 0, 0 };
			accelation		= { 0, 0, 0 };

		}
	};

	struct Emitter
	{
		float		timer;

		E_EffectType type;

		string	sprite_id;

		E_EmitType	emit_type;
		int			emit_per_second;
		int			emit_once;
		float		emit_time;

		float		life_time[2];
		
		E_EmitterAttributeType color_setting_type;
		E_EmitterAttributeType size_setting_type;
		E_EmitterAttributeType rotation_setting_type;
		E_EmitterAttributeType position_setting_type;

		// COLOR
			// INITIAL_SET
			XMFLOAT4	initial_color;
			// SET_PER_LIFETIME
			XMFLOAT4			color_timeline[EFFECT_TIMELINE_SIZE];
			map<int, XMFLOAT4>	color_timeline_map;

		// SIZE
			// INITIAL_SET
			XMFLOAT3	initial_size[2];
			// ADD_PER_LIFETIME
			XMFLOAT3	add_size_per_lifetime[2];
			// SET_PER_LIFETIME
			XMFLOAT3			size_timeline[EFFECT_TIMELINE_SIZE];
			map<int, XMFLOAT3>	size_timeline_map;

		// ROTATION
			// INITIAL_SET	
			XMFLOAT3			initial_rotation[2];
			// ADD_PER_LIFETIME
			XMFLOAT3			add_rotation_per_lifetime[2];
			// SET_PER_LIFETIME
			XMFLOAT3			rotation_timeline[EFFECT_TIMELINE_SIZE];
			map<int, XMFLOAT3>	rotation_timeline_map;

		// POSITION
			// INITIAL_SET
			XMFLOAT3	initial_position[2];
			XMFLOAT3	initial_velocity[2];
			// ADD_PER_LIFETIME
			XMFLOAT3	accelation_per_lifetime[2];
			// SET_PER_LIFETIME
			XMFLOAT3				velocity_timeline[EFFECT_TIMELINE_SIZE];
			map<int, XMFLOAT3>	velocity_timeline_map;

		// GRAVITY
			// ON_OFF
			bool gravity_on_off;

		string		vs_id;
		string		geo_id;
		string		mat_id;

		E_EffectBS	bs_state;
		E_EffectDS  ds_state;

		vector<Particle> particles;

		Emitter()
		{
			type = EMITTER;
			sprite_id = "";

			timer = 0.0f;

			type = NONE;

			emit_type = PER_SECOND;

			emit_once = 0;
			emit_per_second = 0;
			emit_time = 0.0f;

			ZeroMemory(life_time, sizeof(float) * 2);

			color_setting_type		= INITIAL_SET;
			size_setting_type		= INITIAL_SET;
			rotation_setting_type	= INITIAL_SET;
			position_setting_type	= INITIAL_SET;

			// COLOR
			initial_color = { 1.0, 1.0f, 1.0f, 1.0f };
			ZeroMemory(color_timeline, sizeof(XMFLOAT4) * EFFECT_TIMELINE_SIZE);

			// SIZE
			ZeroMemory(initial_size,			sizeof(XMFLOAT3) * 2);
			ZeroMemory(add_size_per_lifetime,	sizeof(XMFLOAT3) * 2);
			ZeroMemory(size_timeline,			sizeof(XMFLOAT3) * EFFECT_TIMELINE_SIZE);

			// ROTATION
			ZeroMemory(initial_rotation,			sizeof(XMFLOAT3) * 2);
			ZeroMemory(add_rotation_per_lifetime,	sizeof(XMFLOAT3) * 2);
			ZeroMemory(rotation_timeline,			sizeof(XMFLOAT3) * EFFECT_TIMELINE_SIZE);

			// POSITION
			ZeroMemory(initial_position,		sizeof(XMFLOAT3) * 2);
			ZeroMemory(initial_velocity,		sizeof(XMFLOAT3) * 2);
			ZeroMemory(accelation_per_lifetime, sizeof(XMFLOAT3) * 2);
			ZeroMemory(velocity_timeline,		sizeof(XMFLOAT3) * EFFECT_TIMELINE_SIZE);

			// GRAVITY
			gravity_on_off = false;

			vs_id = "";
			geo_id = "";
			mat_id = "";

			bs_state = DEFAULT_BS;
			ds_state = DEFAULT_NONE;
		}
	};

	struct Effect
	{
		map<string, Emitter> emitters;
	};

	// UI
	enum class E_UIState
	{
		UI_NORMAL = 0,
		UI_HOVER = 1,
		UI_PUSH = 2,
		UI_SELECT = 3,
		UI_DISABLED = 4,
	};

	struct Rect
	{
		XMFLOAT2	min;
		XMFLOAT2	max;
		XMFLOAT2	center;
		float		width;
		float		height;
		void SetRectByMin(XMFLOAT2 min, float width, float height)
		{
			this->min = min;
			this->width = width;
			this->height = height;
			this->max = { min.x + width, min.y + height };
			this->center = { min.x + width / 2.0f, min.y + height / 2.0f };
		}

		void SetRectByMax(XMFLOAT2 max, float width, float height)
		{
			this->max = max;
			this->width = width;
			this->height = height;
			this->min = { max.x - width, max.y - height };
			this->center = { max.x - width / 2.0f, max.y - height / 2.0f };
		}

		void SetRectByCenter(XMFLOAT2 center, float width, float height)
		{
			this->center = center;
			this->width = width;
			this->height = height;
			this->min = { center.x - width / 2.0f, center.y - width / 2.0f };
			this->max = { center.x + width / 2.0f, center.y + width / 2.0f };
		}

		void SetRectByMinMax(XMFLOAT2 min, XMFLOAT2 max)
		{
			this->min = min;
			this->max = max;
			this->width = max.x - min.x;
			this->height = max.y - min.y;
			this->center = { min.x + (this->width / 2.0f), min.y + (this->height / 2.0f) };
		}
	};

	struct CbUI
	{
		__declspec(align(16)) struct Data
		{
			XMMATRIX world;
		} data;
		ComPtr<ID3D11Buffer> buffer;
	};

	struct RectTransform
	{
		Rect world_rect;
		Rect local_rect;
		XMMATRIX local_matrix;
		XMMATRIX world_matrix;
	};

	struct RectRenderData
	{
		string vs_id;
		string ps_id;
		string tex_id;
		vector<UIVertex>		vertex_list;
		ComPtr<ID3D11Buffer>	vertex_buffer;
		vector<DWORD>			index_list;
		ComPtr<ID3D11Buffer>	index_buffer;
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
