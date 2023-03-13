#include "include/PixelCommon.hlsli"

struct PS_OUT
{
    float4 p : SV_POSITION;
    float4 n : NORMAL;
    float4 c : COLOR;
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
    float4 texcolor = textures.SampleLevel(sample, output.t, 0); //CreateColor(textures, sample, output.t);
    
    float4 skycolor = sky_color;
    
    if (strength.w > 0.0f) // baxkgorung sky
    {
        if (output.origin.y >= 0)
        {
            skycolor.x += output.origin.y * strength.w;
            skycolor.y += output.origin.y * strength.w;
            skycolor.z += output.origin.y * strength.w;
        }
        
        return skycolor;
    }
    else
    {
        texcolor.a = length(texcolor);
        return texcolor;
    }
}