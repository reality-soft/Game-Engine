#include "include/PixelCommon.hlsli"

struct PS_OUT
{
    float4 p : SV_POSITION;
    float4 n : NORMAL;
    float2 t : TEXCOORD;
    float3 origin : TEXCOORD1;
};

cbuffer cb_time : register(b1)
{
    float4 time;  
    float4 sky_color;
    float4 strength;
}

Texture2D textures : register(t0);
SamplerState sample : register(s0);

float4 PS(PS_OUT output) : SV_Target  
{  
    float4 texcolor = textures.SampleLevel(sample, output.t, 0);
    float4 skycolor = sky_color;
    if (output.origin.y >= 0)
    {        
        skycolor.x -= pow(output.origin.y, strength.w) * 0.5;
        skycolor.y -= pow(output.origin.y, strength.w) * 0.5;
        skycolor.z -= pow(output.origin.y, strength.w) * 0.5;
    }
        
    return skycolor;
}