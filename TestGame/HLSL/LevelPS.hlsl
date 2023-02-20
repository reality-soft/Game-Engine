#include "LevelHeader.hlsli"


cbuffer cb_light : register(b0)
{
	float4 default_light;
	float light_bright;
}

Texture2D    g_txTex			: register(t0);
SamplerState g_SampleWrap		: register(s0);

float4 PS(GS_IN output) : SV_Target
{
	// Tex
	float4 tex_color = g_txTex.Sample(g_SampleWrap, output.t);

	// Light
	float4 light = default_light * light_bright;
	float bright = max(0.2f, dot(output.n, -default_light));
	float4 normalmap = { bright , bright , bright , 1};

	return output.c * tex_color * normalmap;
}