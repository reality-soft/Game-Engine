#include "PSCommon.hlsli"

struct PS_OUT
{
    float4 p : SV_POSITION;
    float4 n : NORMAL;
    float4 c : COLOR;
    float2 t : TEXCOORD;
    uint   m : MATERIAL;
};

cbuffer cb_light : register(b0)
{
    float4 direction;
    float4 color;
}

Texture2D diffuse[7] : register(t0);

SamplerState samper_state : register(s0);

float4 PS(PS_OUT output) : SV_Target
{
	// Tex	
    float4 base_color;
    if (output.m == 0)
        base_color = CreateColor(diffuse[0], samper_state, output.t);
    if (output.m == 1)
        base_color = CreateColor(diffuse[1], samper_state, output.t);
    if (output.m == 2)
        base_color = CreateColor(diffuse[2], samper_state, output.t);
    if (output.m == 3)
        base_color = CreateColor(diffuse[3], samper_state, output.t);
    if (output.m == 4)
        base_color = CreateColor(diffuse[4], samper_state, output.t);
    if (output.m == 5)
        base_color = CreateColor(diffuse[5], samper_state, output.t);
    if (output.m == 6)
        base_color = CreateColor(diffuse[6], samper_state, output.t);

    
    
	// Light 
    float4 light = -direction * color;
    float bright = max(0.2f, dot(output.n, light));
    float4 normal = { bright, bright, bright, 1 };

    return output.c * base_color * normal;
}