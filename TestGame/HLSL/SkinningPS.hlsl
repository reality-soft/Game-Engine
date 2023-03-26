#include "include/PixelCommon.hlsli"

struct PS_IN
{
	float3 p : POSITION;
	float3 n : NORMAL;
	float4 c : COLOR;
	float2 t : TEXTURE;
	float lod : TEXCOORD1;
};
struct PS_OUT
{
	float4 p : SV_POSITION;
	float4 n : NORMAL;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
	float lod : TEXCOORD1;
};

Texture2D    g_txTex			: register(t0);
SamplerState g_SampleWrap		: register(s0);

float4 PS(PS_OUT input) : SV_Target
{
	//float4 vColor = g_txTex.Sample(g_SampleWrap , input.t);

	float4 base_color = g_txTex.SampleLevel(g_SampleWrap, input.t, input.lod);
	// Light 
	float bright = max(0.2f, dot(input.n, -direction));
	float4 light_color = float4(bright, bright, bright, 1);

	return base_color * light_color * input.c;
}