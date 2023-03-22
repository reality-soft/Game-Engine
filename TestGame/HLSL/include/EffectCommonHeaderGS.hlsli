struct GS_IN
{
	float4 p			: SV_POSITION;
	float4 c			: COLOR0;
	float2 t			: TEXCOORD0;
};

struct GS_OUT
{
	float4 p	: SV_POSITION;
	float4 c	: COLOR0;
	float2 t	: TEXCOORD0;
	float2 uv	: TEXCOORD1;
};

cbuffer cb_billboard : register(b0)
{
	float4x4 g_view;
	float4x4 g_proj;
	float4x4 g_billboard;
	float4x4 g_x_billboard;
	float4x4 g_y_billboard;
}

cbuffer cb_effect : register(b1)
{
	float4x4	g_world;
}

cbuffer cb_emitter : register(b2)
{
	//int		g_sprite_type;
	//int		g_max_frame;
	//int		g_uv_list_size;
	//int		g_gravity_onoff (0:false, 1:true);
	int4		g_emitter_values;
	//float		g_start_u[255];
	//float		g_start_v[255];
	//float		g_end_u[255];
	//float		g_end_v[255];
	float4		g_emitter_values2[255];
}

cbuffer cb_particle : register(b3)
{
	// x : timer
	// y : frame_ratio
	float4	g_particle_values;
	float4	g_color;
	matrix  g_mat_particle;
}