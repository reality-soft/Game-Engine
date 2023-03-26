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
    float4 tex_color = textures.Sample(samper_state, output.t);
    tex_color = ChangeSaturation(tex_color, 1.5f);
    tex_color = ChangeValue(tex_color, 0.5f);
    
    float4 light_color = WhiteColor();
    light_color = ApplyDirectionalLight(light_color, output.normal);
    light_color = ApplyAmbientLight(light_color);
    
    
    float4 final_color = ApplySpecularLight(tex_color * light_color, output.view_dir, reflect(direction.xyz, output.normal), 0.8);
    
    
    return final_color;
}