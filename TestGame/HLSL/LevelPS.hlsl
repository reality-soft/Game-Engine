#include "include/PixelCommon.hlsli"

struct PS_OUT
{
    float4 p : SV_POSITION;
    float2 t : TEXCOORD;
    float lod : TEXCOORD1;
    
    float3 view_dir : TEXCOORD2;
    float3 normal : NORMAL0;
    float3 origin : NORMAL1;
};

Texture2D textures : register(t0);
SamplerState samper_state : register(s0);

float4 PS(PS_OUT output) : SV_Target
{    
    float4 diffuse = textures.Sample(samper_state, output.t);
    diffuse = ChangeSaturation(diffuse, 1.8f);
    
    return ApplyCookTorrance(diffuse, 0.6f, 0.2f, output.normal, output.view_dir);
}