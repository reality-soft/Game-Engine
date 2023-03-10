#include "PixelCommon.hlsli"

struct PS_OUT
{
    float4 p : SV_POSITION;
    float3 n : NORMAL;
    float4 c : COLOR;
    float2 t : TEXTURE0;
    float2 layer_texel : F2_TEXEL;
    
    float lod : COLOR1;
};

Texture2D    g_txTex			: register(t0);
SamplerState g_SampleWrap		: register(s0);

float4 PS(PS_OUT output) : SV_Target
{
	// Tex
	float4 tex_color = g_txTex.Sample(g_SampleWrap, output.t);

	// Light
	float bright = max(0.2f, dot(output.n, direction.xyz));
	float4 normalmap = { bright , bright , bright , 1};

    return tex_color * normalmap;
}