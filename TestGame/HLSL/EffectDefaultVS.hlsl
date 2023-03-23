cbuffer cb_viewproj : register(b0)
{
	matrix g_mat_view;
	matrix g_mat_proj;
}

cbuffer cb_data : register(b1)
{
	matrix g_mat_world;
};

struct VS_IN
{
	float3 p : F3_POSITION;
	float4 c : F4_COLOR;
	float2 t : F2_TEXTURE;
};

struct VS_OUT
{
	float4 p		: SV_POSITION;
	float4 c		: COLOR0;
	float2 t		: TEXCOORD0;
};

VS_OUT VS(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.p		= float4(input.p, 1.0f);
	output.c		= input.c;
	output.t		= input.t;

	return output;
}