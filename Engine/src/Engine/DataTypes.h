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

	template <typename VertexType>
	struct SingleMesh
	{
		vector<VertexType> vertices;
		ComPtr<ID3D11Buffer> vertex_buffer;
		vector<UINT> indices;
		ComPtr<ID3D11Buffer> index_buffer;
	};

	struct CbTransform
	{
		struct Data
		{
			XMMATRIX world_matrix;
		} data;

		ComPtr<ID3D11Buffer> buffer;
	};

	struct CbViewProj
	{
		struct Data
		{
			XMMATRIX view_matrix;
			XMMATRIX projection_matrix;
		} data;
		ComPtr<ID3D11Buffer> buffer;
	};

	struct CbSkeleton
	{
		struct Data
		{
			XMMATRIX  mat_skeleton[255];
		} data;
		ComPtr<ID3D11Buffer> buffer;
	};

	struct CbLight
	{
		struct Data
		{
			XMVECTOR light_direction;
			float light_bright;
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

	// Effect
	enum E_Sprite
	{
		DEFAULT_SPRITE = 0,
		UV = 1,
		TEX = 2,
	};

	enum E_Effect
	{
		NONE = 0,
		UV_SPRITE = 1,
		TEX_SPRITE = 2,
		EMITTER = 3,
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
		XMFLOAT3	initial_position[2];

		XMFLOAT3	size_per_lifetime[2];
		float		rotation_per_lifetime[2];
		XMFLOAT3	velocity_per_lifetime[2];

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
			ZeroMemory(initial_position, sizeof(XMFLOAT3) * 2);


			ZeroMemory(size_per_lifetime, sizeof(XMFLOAT3) * 2);
			ZeroMemory(rotation_per_lifetime, sizeof(float) * 2);
			ZeroMemory(velocity_per_lifetime, sizeof(XMFLOAT3) * 2);

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

static std::wstring to_mw(const std::string& _src)
{
	USES_CONVERSION;
	return std::wstring(A2W(_src.c_str()));
}

static std::string to_wm(const std::wstring& _src)
{
	USES_CONVERSION;
	return std::string(W2A(_src.c_str()));
}

static std::vector<std::string> split(std::string input, char delimiter) {
	std::vector<std::string> answer;
	std::stringstream ss(input);
	std::string temp;

	while (getline(ss, temp, delimiter)) {
		answer.push_back(temp);
	}

	return answer;
}


static void XMtoRP(XMVECTOR& xmv, reactphysics3d::Vector3& rpv)
{
	rpv.x = xmv.m128_f32[0];
	rpv.y = xmv.m128_f32[1];
	rpv.z = xmv.m128_f32[2];
}

static void XMtoRP(XMVECTOR& xmv, reactphysics3d::Vector2& rpv)
{
	rpv.x = xmv.m128_f32[0];
	rpv.y = xmv.m128_f32[1];
}

static void RPtoXM(reactphysics3d::Vector3& rpv, XMVECTOR& xmv)
{
	xmv.m128_f32[0] = rpv.x;
	xmv.m128_f32[1] = rpv.y;
	xmv.m128_f32[2] = rpv.z;
}

static void RPtoXM(reactphysics3d::Vector2& rpv, XMVECTOR& xmv)
{
	xmv.m128_f32[0] = rpv.x;
	xmv.m128_f32[1] = rpv.y;
}

#define randf(x) (x*rand()/(float)RAND_MAX)
#define randf2(x,off) (off+x*rand()/(float)RAND_MAX)
#define randstep(fMin,fMax) (fMin+((float)fMax-(float)fMin)*rand()/(float)RAND_MAX)