#pragma once
#include "stdafx.h"

namespace reality
{
	// Effect

#define EFFECT_TIMELINE_SIZE 101

	enum E_EffectType
	{
		NONE = 0,
		UV_SPRITE = 1,
		TEX_SPRITE = 2,
		EMITTER = 3,
		EFFECT = 5,
	};

	enum E_EmitterAttributeType
	{
		INITIAL_SET = 0,
		ADD_PER_LIFETIME = 1,
		SET_PER_LIFETIME = 2,
	};

	enum E_EffectBS
	{
		DEFAULT_BS = 0,
		NO_BLEND = 1,
		ALPHA_BLEND = 2,
		DUALSOURCE_BLEND = 3,
		HIGHER_RGB = 4,
	};

	enum E_EffectDS
	{
		DEFAULT_NONE = 0,
		DEPTH_COMP_NOWRITE = 1,
		DEPTH_COMP_WRITE = 2,
	};

	enum E_EmitType
	{
		PER_SECOND = 0,
		ONCE = 1,
		AFTER_TIME = 2,
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
			//int b_gravity_onoff(0:off/1:on);
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
			XMMATRIX transform_for_billboard;
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
		XMFLOAT3	add_rotation;
		XMFLOAT3	accelation;

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
			velocity = { 0, 0, 0 };

			add_size = { 0, 0, 0 };
			add_rotation = { 0, 0, 0 };
			accelation = { 0, 0, 0 };

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

			color_setting_type = INITIAL_SET;
			size_setting_type = INITIAL_SET;
			rotation_setting_type = INITIAL_SET;
			position_setting_type = INITIAL_SET;

			// COLOR
			initial_color = { 1.0, 1.0f, 1.0f, 1.0f };
			ZeroMemory(color_timeline, sizeof(XMFLOAT4) * EFFECT_TIMELINE_SIZE);

			// SIZE
			ZeroMemory(initial_size, sizeof(XMFLOAT3) * 2);
			ZeroMemory(add_size_per_lifetime, sizeof(XMFLOAT3) * 2);
			ZeroMemory(size_timeline, sizeof(XMFLOAT3) * EFFECT_TIMELINE_SIZE);

			// ROTATION
			ZeroMemory(initial_rotation, sizeof(XMFLOAT3) * 2);
			ZeroMemory(add_rotation_per_lifetime, sizeof(XMFLOAT3) * 2);
			ZeroMemory(rotation_timeline, sizeof(XMFLOAT3) * EFFECT_TIMELINE_SIZE);

			// POSITION
			ZeroMemory(initial_position, sizeof(XMFLOAT3) * 2);
			ZeroMemory(initial_velocity, sizeof(XMFLOAT3) * 2);
			ZeroMemory(accelation_per_lifetime, sizeof(XMFLOAT3) * 2);
			ZeroMemory(velocity_timeline, sizeof(XMFLOAT3) * EFFECT_TIMELINE_SIZE);

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

	struct BaseLight
	{
		XMFLOAT4	light_color;
		float		range;
		XMFLOAT3	attenuation;
		float		specular;
	};

	struct PointLight : public BaseLight
	{
		
	};

	struct SpotLight : public BaseLight
	{
		float		spot;
	};
}