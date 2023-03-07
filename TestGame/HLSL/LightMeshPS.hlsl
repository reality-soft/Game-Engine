#include "include/PixelCommon.hlsli"

struct PS_OUT
{
    float4 p : SV_POSITION;
    float2 t : TEXCOORD;
    float lod : TEXCOORD1;
};

Texture2D textures : register(t0);
SamplerState samper_state : register(s0);

float4 PS(PS_OUT output) : SV_Target
{    
	// Tex	
    float4 base_color = textures.Sample(samper_state, output.t);
    return base_color;
}