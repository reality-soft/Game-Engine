#include "include/PixelCommon.hlsli"

struct PS_OUT
{
    float4 p : SV_POSITION;
    float2 t : TEXCOORD;
    float lod : TEXCOORD1;
    
    float3 view_dir : TEXCOORD2;
    
    float3 normal : NORMAL0;
    float3 origin : NORMAL1;
    //float3 tangent;
    //float3 binormal;
};

Texture2D textures : register(t0);
SamplerState samper_state : register(s0);

float4 PS(PS_OUT output) : SV_Target
{
    float4 base_color = textures.Sample(samper_state, output.t);    
    float4 ambient_color = float4(0.1f, 0.1f, 0.1f, 0.1f);
    float4 specular_color = float4(0, 0, 0, 0);
    float light_intensity = dot(output.normal, direction.xyz);
    
    float4 color = ambient_color;
    if (light_intensity > 0.0f)
    {
        ambient_color *= light_intensity;
        //float3 reflection = normalize(2 * light_intensity * output.normal - direction.xyz);
        //specular_color = dot(reflection, output.view_dir);
    }
    
    
    base_color += light_intensity * 0.1f;
    //color += specular_color;

    return base_color;
    //float sun_distance = 1.0f - length(output.origin - sun_position.xyz) / (length(sun_position.xyz) * 2);    
    //return base_color + light_color;
    //light_color;
}