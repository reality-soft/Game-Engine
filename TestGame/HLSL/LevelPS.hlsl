#include "include/PixelCommon.hlsli"

struct PS_OUT
{
    float4 p : SV_POSITION;
    float3 n : NORMAL0;
    float2 t : TEXCOORD;
    
    float lod : TEXCOORD1;    
    float3 view_dir : TEXCOORD2;
    float3 origin : NORMAL1;
};

Texture2D textures[7] : register(t0);
SamplerState samper_state : register(s0);

float4 PS(PS_OUT input) : SV_Target
{   
    float4 albedo = textures[0].Sample(samper_state, input.t);
    float4 roughness = textures[3].Sample(samper_state, input.t);
    
    float4 final_color = WhiteColor();
    
    albedo = ChangeSaturation(albedo, 1.8f);
    albedo = ApplyHemisphericAmbient(input.n, albedo);
    
    final_color = ApplyCookTorrance(albedo, roughness.r, specular_strength, input.n, input.view_dir);
    final_color = ApplyDistanceFog(final_color, input.origin);

    return final_color;
}